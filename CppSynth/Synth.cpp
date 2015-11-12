#include <stdio.h>
#include <math.h>
#include <cmath>
#include <iostream>
#include <chrono>

#include "Synth.h"
#include "Osc/OscMessage.h"
#include "AudioLib/Utils.h"
#include "PlatformSpecific.h"

using namespace std;
using namespace AudioLib;

namespace Leiftur
{
	// ------------------- Interface Methods ----------------------

	Synth::Synth()
	{
		wavetableManager = make_shared<WavetableManager>();
		isClosing = false;
		masterVol = 1.0;
		udpTranceiver = 0;
		outputBufferL = 0;
		outputBufferR = 0;
		voiceAllocator.Initialize(Voices);

		for (int i = 0; i < MaxVoiceCount; i++)
			VoiceStates.push_back(0);
	}

	Synth::~Synth()
	{
		isClosing = true;
		delete outputBufferL;
		delete outputBufferR;

		if (messageListenerThread.joinable())
			messageListenerThread.join();

		delete udpTranceiver;
		udpTranceiver = 0;
	}

	void Synth::Initialize(int samplerate, bool oversample, int udpListenPort, int udpSendPort)
	{
		if (udpListenPort != 0)
		{
			delete udpTranceiver;
			udpTranceiver = new UdpTranceiver(udpListenPort, udpSendPort);
			messageListenerThread = thread(&Synth::MessageListener, this);
		}

		this->samplerate = samplerate;
		this->oversampling = oversample ? 2 : 1;
		outputBufferL = new float[BufferSize];
		outputBufferR = new float[BufferSize];
		wavetableManager->Setup(PlatformSpecific::GetDllDir());
		presetManager.Initialize(PlatformSpecific::GetDllDir());
		Delay.Initialize(samplerate * oversampling, BufferSize, ModulationUpdateRate);
				
		for (size_t i = 0; i < MaxVoiceCount; i++)
		{
			Voices[i].Initialize(samplerate * oversampling, ModulationUpdateRate * oversampling, BufferSize, wavetableManager);
		}

		LoadPreset(presetManager.GetDefaultPreset());
		isReady = true;
	}

	void Synth::SetParameter(int key, double value)
	{
		Module module;
		int parameter;
		UnpackParameter(key, &module, &parameter);
		SetParameterInner(module, parameter, value);
	}

	void Synth::ProcessMidi(uint8_t* message)
	{
		int msgType = (message[0] & 0xF0);
		int channel = (message[0] & 0x0F);

		if (msgType == 0x80)
		{
			NoteOff(message[1]);
		}
		else if (msgType == 0x90)
		{
			NoteOn(message[1], message[2] / 127.0f);
		}
		else if (msgType == 0xA0)
		{
			SetKeyPressure(message[1], message[2] / 127.0f);
		}
		else if (msgType == 0xB0)
		{
			MidiCC(message[1], message[2]);
		}
		else if (msgType == 0xC0)
		{
			MidiProgram(message[1]);
		}
		else if (msgType == 0xD0)
		{
			SetChannelPressure(message[1] / 127.0f);
		}
		else if (msgType == 0xE0)
		{
			int pitch = (message[1] | (message[2] << 7)) - 0x2000;
			float fPitch = pitch / 8192.0f;
			PitchWheel(fPitch);
		}
	}

	void Synth::ProcessAudio(float** buffer, int outputBufferLen)
	{
		int n = 0;
		int totalOversampledToProcess = outputBufferLen * oversampling;
		while (n < totalOversampledToProcess)
		{
			int bufSize = BufferSize;
			if ((totalOversampledToProcess - n) < bufSize)
				bufSize = totalOversampledToProcess - n;

			int voiceCount = voiceAllocator.GetVoiceCount();
			
			Utils::ZeroBuffer(outputBufferL, bufSize);
			Utils::ZeroBuffer(outputBufferR, bufSize);

			for (int i = 0; i < voiceCount; i++)
			{
				Voices[i].Process(bufSize);
				auto output = Voices[i].GetOutput();
				Utils::GainAndSum(output[0], outputBufferL, masterVol, bufSize);
				Utils::GainAndSum(output[1], outputBufferR, masterVol, bufSize);
			}

			if (moduleSwitches[(int)ModuleSwitchParameters::DelayOn])
			{
				Delay.Process(outputBufferL, outputBufferR, bufSize);
				Utils::Copy(Delay.GetOutputL(), outputBufferL, bufSize);
				Utils::Copy(Delay.GetOutputR(), outputBufferR, bufSize);
			}
			
			// Decimate and copy output to out buffer
			if (oversampling == 2)
			{
				float* leftOut = &buffer[0][n / oversampling];
				float* rightOut = &buffer[1][n / oversampling];
				for (int i = 0; i < bufSize / oversampling; i++)
				{
					float outL = decimatorL.Process(outputBufferL[2 * i], outputBufferL[2 * i + 1]);
					float outR = decimatorR.Process(outputBufferR[2 * i], outputBufferR[2 * i + 1]);

					leftOut[i] = outL;
					rightOut[i] = outR;
				}
			}
			else
			{
				float* leftOut = &buffer[0][n];
				float* rightOut = &buffer[1][n];
				for (int i = 0; i < bufSize; i++)
				{
					leftOut[i] = outputBufferL[i];
					rightOut[i] = outputBufferR[i];
				}
			}

			n += bufSize;
		}
	}


	// ------------------------------------------------------------------------------
	// ------------------------------ Inner Methods ---------------------------------
	// ------------------------------------------------------------------------------


	void Synth::MessageListener()
	{
		auto sleepTime = chrono::milliseconds(2);

		while (!isClosing)
		{
			try
			{
				while (true)
				{
					if (!isReady)
						break;

					auto data = udpTranceiver->Receive();
					if (data.size() == 0)
						break;
					
					try
					{
						auto oscMsgs = OscMessage::ParseRawBytes(data);
						auto oscMsg = oscMsgs.at(0);
						Module module;
						int parameter;
						Parameters::ParseAddress(oscMsg.Address, &module, &parameter);
						if (module == Module::Control)
						{
							HandleControlMessage(oscMsg);
						}
						else if (oscMsg.TypeTags.at(0) == 'f')
						{
							float value = oscMsg.GetFloat(0);
							SetParameterInner(module, parameter, value);
						}
					}
					catch (exception ex)
					{
						try
						{
							std::cout << ex.what() << std::endl;
							OscMessage oscMsg("/Control/ErrorMessage");
							oscMsg.SetString(std::string("An Error occurred while processing an OSC message:\n") + ex.what());
							udpTranceiver->Send(oscMsg.GetBytes());
						}
						catch (exception ex2)
						{
							std::cout << ex2.what() << std::endl;
						}
					}
				}

				SendVoiceStates();
			}
			catch (exception ex)
			{
				std::cout << ex.what() << std::endl;
			}

			this_thread::sleep_for(sleepTime);
		}
	}
	
	void Synth::HandleControlMessage(OscMessage msg)
	{
		if (msg.Address == "/Control/RequestState")
			SendStateToEditor();
		else if (msg.Address == "/Control/RequestWaveforms")
			SendWaveformsToEditor();
		else if (msg.Address == "/Control/RequestBanks")
			SendBanksToEditor();
		else if (msg.Address == "/Control/RequestPresets")
			SendPresetsToEditor(msg.GetString(0));
		else if (msg.Address == "/Control/LoadPreset")
			LoadPreset(msg.GetString(0), msg.GetString(1));
		else if (msg.Address == "/Control/SavePreset")
			SavePreset(msg.GetString(0), msg.GetString(1));
		else if (msg.Address == "/Control/RequestVisual/EnvAmp")
			SendVisual(Module::EnvAmp);
	}
	
	void Synth::LoadPreset(std::string bank, std::string presetName)
	{
		auto preset = presetManager.GetPreset(bank, presetName);
		LoadPreset(preset);
	}

	void Synth::LoadPreset(Preset preset)
	{
		currentPreset = preset;

		// we need to restore the waveform id from the selector (the string), as the ID can change when wav files are added and deleted
		currentPreset.Values[PackParameter(Module::Osc1, (int)OscParameters::Waveform)] = wavetableManager->GetId(currentPreset.Metadata[PresetManager::Osc1Waveform]);
		currentPreset.Values[PackParameter(Module::Osc2, (int)OscParameters::Waveform)] = wavetableManager->GetId(currentPreset.Metadata[PresetManager::Osc2Waveform]);
		currentPreset.Values[PackParameter(Module::Osc3, (int)OscParameters::Waveform)] = wavetableManager->GetId(currentPreset.Metadata[PresetManager::Osc3Waveform]);
		
		for (std::map<int, double>::iterator it = currentPreset.Values.begin(); it != currentPreset.Values.end(); ++it)
		{
			const int key = it->first;
			const double value = it->second;
			Module module;
			int parameter;
			UnpackParameter(key, &module, &parameter);
			SetParameterInner(module, parameter, value);
		}

		SendStateToEditor();
	}

	void Synth::SendStateToEditor()
	{
		OscMessage msg("/Control/PresetData");
		msg.SetString(currentPreset.Serialize());
		udpTranceiver->Send(msg.GetBytes());

		for (std::map<int, double>::iterator it = currentPreset.Values.begin(); it != currentPreset.Values.end(); ++it)
		{
			const int key = it->first;
			Module module;
			int parameter;
			UnpackParameter(key, &module, &parameter);
			SendBackParameter(module, parameter);
		}
	}

	void Synth::SendWaveformsToEditor()
	{
		auto& files = wavetableManager->WavetableFiles;

		auto msg = OscMessage("/Control/Waveforms");
		int i = 0;

		for (auto file : files)
		{
			msg.SetInt(file.Index);
			msg.SetString(file.Selector);
			i++;
			if (i >= 2) // send chunks of waveforms, as we can have alot and it might not fit in a single udp message
			{
				udpTranceiver->Send(msg.GetBytes());
				msg = OscMessage("/Control/Waveforms");
				i = 0;
			}
		}

		if (i > 0)
			udpTranceiver->Send(msg.GetBytes());
	}

	void Synth::SendBanksToEditor()
	{
		auto bankNames = presetManager.GetBankString();
		OscMessage msg("/Control/Banks");
		msg.SetString(bankNames);
		udpTranceiver->Send(msg.GetBytes());
	}

	void Synth::SendPresetsToEditor(std::string bankName)
	{
		auto presetNames = presetManager.GetPresetString(bankName);
		OscMessage msg("/Control/Presets");
		msg.SetString(bankName);
		msg.SetString(presetNames);
		udpTranceiver->Send(msg.GetBytes());
	}

	void Synth::SavePreset(std::string bankName, std::string presetName)
	{
		currentPreset.BankName = bankName;
		currentPreset.PresetName = presetName;

		// we need to store the waveform selector (the string), as the ID can change when wav files are added and deleted
		currentPreset.Metadata[PresetManager::Osc1Waveform]
			= wavetableManager->WavetableFiles[Parameters::FloorToInt(currentPreset.Values[PackParameter(Module::Osc1, (int)OscParameters::Waveform)])].Selector;
		currentPreset.Metadata[PresetManager::Osc2Waveform]
			= wavetableManager->WavetableFiles[Parameters::FloorToInt(currentPreset.Values[PackParameter(Module::Osc2, (int)OscParameters::Waveform)])].Selector;
		currentPreset.Metadata[PresetManager::Osc3Waveform]
			= wavetableManager->WavetableFiles[Parameters::FloorToInt(currentPreset.Values[PackParameter(Module::Osc3, (int)OscParameters::Waveform)])].Selector;

		presetManager.SavePreset(&currentPreset);
		OscMessage msg("/Control/PresetsChanged");
		msg.SetString(bankName);
		udpTranceiver->Send(msg.GetBytes());
	}

	void Synth::SendVisual(Module module)
	{
		if (module == Module::EnvAmp)
		{
			auto vis = Voices[0].ampEnv.GetVisual();
			OscMessage msg("/Control/Visual/EnvAmp");
			msg.SetBlob(vis);
			udpTranceiver->Send(msg.GetBytes());
		}
		else if (module == Module::EnvFilter)
		{
			auto vis = Voices[0].filterEnv.GetVisual();
			OscMessage msg("/Control/Visual/EnvFilter");
			msg.SetBlob(vis);
			udpTranceiver->Send(msg.GetBytes());
		}
	}

	void Synth::SendVoiceStates()
	{
		bool shouldSend = false;

		for (int i = 0; i < MaxVoiceCount; i++)
		{
			int state = Voices[i].GetState();
			if (state != VoiceStates[i])
				shouldSend = true;

			VoiceStates[i] = state;
		}

		if (shouldSend)
		{
			OscMessage msg("/Control/VoiceState");
			msg.SetBlob(VoiceStates);
			udpTranceiver->Send(msg.GetBytes());
		}
	}

	void Synth::SetParameterInner(Module module, int parameter, double value)
	{
		int idx = PackParameter(module, parameter);
		currentPreset.Values[idx] = value;
		SendBackParameter(module, parameter);

		if (module == Module::Voices)
			SetGlobalVoiceParameter((VoiceParameters)parameter, value);
		else if (module == Module::ModuleSwitches)
			SetGlobalModuleSwitchParameter((ModuleSwitchParameters)parameter, value);


		if (module == Module::Delay)
		{
			Delay.SetParameter((DelayParameters)parameter, value);
		}
		else
		{
			for (size_t i = 0; i < MaxVoiceCount; i++)
			{
				Voices[i].SetParameter(module, parameter, value);
			}
		}
	}

	void Synth::SetGlobalVoiceParameter(VoiceParameters parameter, double value)
	{
		if (parameter == VoiceParameters::Unison)
		{
			int val = Parameters::FloorToInt(value);
			voiceAllocator.unison = val < 1 ? 1 : val;
			voiceAllocator.UpdateVoiceStates();
		}
		else if (parameter == VoiceParameters::Master)
		{
			masterVol = (float)value;
		}
		else if (parameter == VoiceParameters::Polyphony)
		{
			int val = Parameters::FloorToInt(value);
			if (val < 1) val = 1;
			if (val > MaxVoiceCount) val = MaxVoiceCount;
			voiceAllocator.polyphony = val;
			voiceAllocator.UpdateVoiceStates();
		}
		else if (parameter == VoiceParameters::VoiceMode)
		{
			voiceAllocator.voiceMode = (VoiceMode)(int)Parameters::FloorToInt(value);
			voiceAllocator.UpdateVoiceStates();
		}
	}

	void Synth::SetGlobalModuleSwitchParameter(ModuleSwitchParameters parameter, double value)
	{
		moduleSwitches[(int)parameter] = value >= 0.5;
	}

	// ------------------------------------------------------------------------------
	// ------------------------------------------------------------------------------
	// ------------------------------------------------------------------------------


	void Synth::NoteOn(uint8_t note, float velocity)
	{
		voiceAllocator.NoteOn(note, velocity);
	}

	void Synth::NoteOff(uint8_t note)
	{
		voiceAllocator.NoteOff(note);
	}

	void Synth::MidiCC(uint8_t byte1, uint8_t byte2)
	{
		if (byte1 == 1)
		{
			for (size_t i = 0; i < MaxVoiceCount; i++)
				Voices[i].SetModWheel(byte2 / 127.0f);
		}
	}

	void Synth::MidiProgram(uint8_t program)
	{
	}

	void Synth::PitchWheel(float pitchbend)
	{
		for (size_t i = 0; i < MaxVoiceCount; i++)
			Voices[i].SetPitchWheel(pitchbend);
	}

	void Synth::SetModWheel(float value)
	{
		for (size_t i = 0; i < MaxVoiceCount; i++)
			Voices[i].SetModWheel(value);
	}

	void Synth::SetKeyPressure(int note, float pressure)
	{
		for (size_t i = 0; i < MaxVoiceCount; i++)
		{
			if (Voices[i].Note == note)
				Voices[i].SetKeyPressure(pressure);
		}
	}

	void Synth::SetChannelPressure(float pressure)
	{
		for (size_t i = 0; i < MaxVoiceCount; i++)
			Voices[i].SetModWheel(pressure);
	}

}
