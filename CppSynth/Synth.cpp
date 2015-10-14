#include <stdio.h>
#include <math.h>
#include <cmath>
#include <iostream>

#include "Synth.h"
#include "Osc/OscMessage.h"

Synth::Synth()
{
	udpTranceiver = 0;
}

Synth::~Synth()
{
	delete udpTranceiver;
	udpTranceiver = 0;
}

void Synth::Initialize(int samplerate, int udpListenPort, int udpSendPort)
{
	if (udpListenPort != 0)
	{
		delete udpTranceiver;
		udpTranceiver = new UdpTranceiver(udpListenPort, udpSendPort);

		while (true)
		{
			auto data = udpTranceiver->Receive();
			auto oscMsgs = OscMessage::ParseBundle(data);
			auto oscMsg = oscMsgs[0];

			if (oscMsg.TypeTags[0] == 'f')
			{
				float val = oscMsg.GetFloat(0);
				std::cout << oscMsg.Address << ": " << val << std::endl;
			}
		}
	}

	this->Samplerate = samplerate;
	for (size_t i = 0; i < MaxVoiceCount; i++)
	{
		Voices[i].Initialize(samplerate);
	}
}

void Synth::SetParameter(int parameter, double value)
{
	for (size_t i = 0; i < MaxVoiceCount; i++)
	{
		Voices[i].SetParameter(parameter, value);
	}
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
	else if (msgType == 0xB0)
	{
		MidiCC(message[1], message[2]);
	}
	else if (msgType == 0xC0)
	{
		MidiProgram(message[1]);
	}
	else if (msgType == 0xE0)
	{
		int pitch = (message[1] | (message[2] << 7)) - 0x2000;
		PitchWheel(pitch);
	}
}

void Synth::ProcessAudio(float** buffer, int bufferSize)
{
	Voices[0].Process(buffer[0], bufferSize);
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
	Voices[0].SetParameter(0, byte2 / 127.0);
}

void Synth::MidiProgram(uint8_t program)
{
}

void Synth::PitchWheel(int pitchbend)
{
	Voices[0].SetPitchWheel(pitchbend / 8192.0);
}

