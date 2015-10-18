#include <stdio.h>
#include <math.h>
#include <cmath>
#include <iostream>
#include <chrono>

#include "Synth.h"
#include "Osc/OscMessage.h"

using namespace std;

namespace Leiftur
{
	// ------------------- Interface Methods ----------------------

	Synth::Synth()
	{
		isClosing = false;
		udpTranceiver = 0;
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
			NoteOn(message[1], message[2]);
		}
		else if (msgType == 0xA0)
		{
			SetKeyPressure(message[1], message[2] / 127.0);
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
			SetChannelPressure(message[1] / 127.0);
		}
		else if (msgType == 0xE0)
		{
			int pitch = (message[1] | (message[2] << 7)) - 0x2000;
			float fPitch = pitch / 8192.0;
			PitchWheel(fPitch);
		}
	}

	void Synth::ProcessAudio(float** buffer, int bufferSize)
	{
		Voices[0].Process(buffer, bufferSize);
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

		for (size_t i = 0; i < MaxVoiceCount; i++)
		{
			Voices[i].SetParameter(module, parameter, value);
		}
	}

	void Synth::NoteOn(char note, char velocity)
	{
		Voices[0].SetNote(note);
		Voices[0].SetGate(velocity / 127.0);
	}

	void Synth::NoteOff(char note)
	{
		if (Voices[0].Note == note)
			Voices[0].SetGate(0);
	}

	void Synth::MidiCC(uint8_t byte1, uint8_t byte2)
	{
		if (byte1 == 1)
		{
			for (size_t i = 0; i < MaxVoiceCount; i++)
				Voices[i].SetModWheel(byte2 / 127.0);
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
