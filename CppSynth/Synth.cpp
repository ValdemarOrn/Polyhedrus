#include <stdio.h>
#include <math.h>
#include <cmath>
#include <iostream>
#include <chrono>

#include "Synth.h"
#include "Osc/OscMessage.h"
#include "AudioLib/Utils.h"

using namespace std;
using namespace AudioLib::Utils;

namespace Leiftur
{
	// ------------------- Interface Methods ----------------------

	Synth::Synth()
	{
		isClosing = false;
		polyphony = 1;
		unison = 1;
		nextVoiceIndex = 0;
		masterVol = 1.0;
		udpTranceiver = 0;
		lastVelocity = 0;

		for (size_t i = 0; i < 128; i++)
		{
			noteCounters[i] = 0;
		}

		noteCounter = 1;
	}

	Synth::~Synth()
	{
		isClosing = true;
		
		if (messageListenerThread.joinable())
			messageListenerThread.join();

		delete udpTranceiver;
		udpTranceiver = 0;
	}

	void Synth::Initialize(int samplerate, int udpListenPort, int udpSendPort)
	{
		if (udpListenPort != 0)
		{
			delete udpTranceiver;
			udpTranceiver = new UdpTranceiver(udpListenPort, udpSendPort);
			messageListenerThread = thread(&Synth::MessageListener, this);
		}

		this->Samplerate = samplerate;
		for (size_t i = 0; i < MaxVoiceCount; i++)
		{
			Voices[i].Initialize(samplerate, ModulationUpdateRate, BufferSize);
		}
	}

	void Synth::SetParameter(int parameter, double value)
	{
		// Parameter values is interpreseted as
		// mmmmmmmmmmmmmmmmxxxxxxxxpppppppp
		// Upper 16 bits = module
		// Lower 8 bits = parameter
		Module module = (Module)((parameter & 0xFF00) >> 16);
		int param = parameter = 0x00FF;
		SetParameterInner(module, param, value, false);
	}

	void Synth::SetParameter(std::string address, double value)
	{
		Module module;
		int parameter;
		Parameters::ParseAddress(address, &module, &parameter);
		SetParameterInner(module, parameter, value, false);
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

	void Synth::ProcessAudio(float** buffer, int bufferSize)
	{
		ZeroBuffer(buffer[0], BufferSize);
		ZeroBuffer(buffer[1], BufferSize);

		for (int i = 0; i < polyphony; i++)
		{
			Voices[i].Process(bufferSize);
			auto output = Voices[i].GetOutput();
			GainAndSum(buffer[0], output[0], 1.0, BufferSize);
			GainAndSum(buffer[1], output[1], 1.0, BufferSize);
		}
	}

	// ------------------- Inner Methods ----------------------

	void Synth::MessageListener()
	{
		auto sleepTime = chrono::milliseconds(1);

		while (!isClosing)
		{
			auto data = udpTranceiver->Receive();

			if (data.size() > 0)
			{
				auto oscMsgs = OscMessage::ParseBundle(data);
				auto oscMsg = oscMsgs[0];
				if (oscMsg.TypeTags[0] == 'f')
				{
					float val = oscMsg.GetFloat(0);
					std::cout << oscMsg.Address << ": " << val << std::endl;
					SetParameter(oscMsg.Address, val);
				}
			}

			this_thread::sleep_for(sleepTime);
		}
	}

	void Synth::SetParameterInner(Module module, int parameter, double value, bool isTranslated)
	{
		//double translatedValue = isTranslated ? value : TranslateValue(module, parameter, value);
		int idx = (((int)module) << 16) | parameter;
		parameters[idx] = value;

		if (module == Module::Voices)
			SetGlobalVoiceParameter((VoiceParameters)parameter, value);
		
		for (size_t i = 0; i < MaxVoiceCount; i++)
		{
			Voices[i].SetParameter(module, parameter, value);
		}
	}

	void Synth::NoteOn(char note, float velocity)
	{
		noteCounters[note] = noteCounter++;
		int newNote = voiceMode == VoiceMode::PolyRoundRobin ? note : FindNextMonoNote();

		for (int i = 0; i < polyphony; i++)
		{
			if (Voices[i].Note == newNote && Voices[i].Gate)
				return; // already playing
		}

		lastVelocity = velocity;
		int unisonActual = (unison > polyphony) ? polyphony : unison;

		for (int i = 0; i < unisonActual; i++)
		{
			int unisonMin = -(unisonActual / 2);
			int unisonMap = unisonMin + i;
			float unisonValue = -unisonMin / (float)unisonMap;
			Voices[nextVoiceIndex].SetNote(note);
			Voices[nextVoiceIndex].SetGate(velocity);
			
			nextVoiceIndex++;
			if (nextVoiceIndex >= polyphony)
				nextVoiceIndex = 0;
		}

		if (voiceMode != VoiceMode::PolyRoundRobin)
			nextVoiceIndex = 0; // Force Monophonic
	}

	void Synth::NoteOff(char note)
	{
		noteCounters[note] = 0;
		int nextNote = voiceMode == VoiceMode::PolyRoundRobin ? -1 : FindNextMonoNote();

		if (nextNote != -1)
		{
			NoteOn(nextNote, lastVelocity);
		}
		if (nextNote == -1)
		{
			for (int i = 0; i < polyphony; i++)
			{
				if (Voices[i].Note == note)
					Voices[i].SetGate(0);
			}
		}
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

	void Synth::SetGlobalVoiceParameter(VoiceParameters parameter, double value)
	{
		if (parameter == VoiceParameters::Unison)
		{
			int val = (int)(value * MaxVoiceCount + 0.001);
			unison = val < 1 ? 1 : val;
		}
		else if (parameter == VoiceParameters::Master)
		{
			masterVol = value;
		}
		else if (parameter == VoiceParameters::Polyphony)
		{
			int val = (int)(value * MaxVoiceCount + 0.001);
			polyphony = val < 1 ? 1 : val;
			UpdateVoiceStates();
		}
		else if (parameter == VoiceParameters::VoiceMode)
		{
			UpdateVoiceStates();
			voiceMode = (VoiceMode)(int)((1.0 - value) * 3.999);
		}
	}

	void Synth::UpdateVoiceStates()
	{
		for (int i = 0; i < MaxVoiceCount; i++)
		{
			if (i >= polyphony)
			{
				Voices[i].SetGate(0.0);
				Voices[i].TurnOff();
			}
		}
	}

	int Synth::FindNextMonoNote()
	{
		if (voiceMode == VoiceMode::MonoHighest)
		{
			for (int i = 127; i >= 0 ; i--)
			{
				if (noteCounters[i] != 0)
					return i;
			}

			return -1;
		}

		if (voiceMode == VoiceMode::MonoLowest)
		{
			for (int i = 0; i < 128; i++)
			{
				if (noteCounters[i] != 0)
					return i;
			}

			return -1;
		}

		if (voiceMode == VoiceMode::MonoNewest)
		{
			int maxCounter = 0;
			int note = -1;
			for (int i = 0; i < 128; i++)
			{
				int counter = noteCounters[i];
				if (counter > maxCounter)
				{
					maxCounter = counter;
					note = i;
				}
			}

			return note;
		}

		return -1;
	}

}
