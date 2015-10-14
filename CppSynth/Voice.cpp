#include "Voice.h"
#include "AudioLib/Utils.h"

namespace Leiftur
{
	Voice::Voice()
	{
	}

	Voice::~Voice()
	{
	}

	void Voice::Initialize(int samplerate)
	{
		this->Samplerate = samplerate;
		osc1.Initialize(samplerate);
	}

	void Voice::SetParameter(Module module, int parameter, double value)
	{
		osc1.WaveIndex = value * 15.99999;
	}

	void Voice::SetGate(float gate)
	{
		Velocity = gate;
	}

	void Voice::SetNote(int note)
	{
		Note = note;
		osc1.Note = note;
	}

	void Voice::SetPitchWheel(float pitchbend)
	{
		osc1.PitchBend = pitchbend;
	}

	void Voice::Process(float * buffer, int bufferSize)
	{
		osc1.GetSamples(buffer, bufferSize);
		AudioLib::Utils::Gain(buffer, Velocity, bufferSize);
	}
}
