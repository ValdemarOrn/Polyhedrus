#include "Lfo.h"
#include "AudioLib/Noise.h"
#include "AudioLib/FastSin.h"

namespace Leiftur
{
	Lfo::Lfo()
	{
		env.SetParameter(EnvParameters::Hold, 0.0);
		env.SetParameter(EnvParameters::Velocity, 0.0);
		iterator = 0;
		gate = false;
		phase = 0.0;
		freq = 1.0;
		sync = false;
		shape = LfoShape::Triangle;
		Update();
	}

	Lfo::~Lfo()
	{
	}

	void Lfo::Initialize(int samplerate)
	{
		this->samplerate = samplerate;
		env.Initialize(samplerate);
	}

	void Lfo::SetParameter(ModParameters parameter, double value)
	{
		switch (parameter)
		{
		case ModParameters::Attack:
			env.SetParameter(EnvParameters::Attack, value);
			return;
		case ModParameters::Decay:
			env.SetParameter(EnvParameters::Decay, value);
			return;
		case ModParameters::Freq:
			freq = GetFrequency(value);
			return;
		case ModParameters::Phase:
			phase = (float)value;
			return;
		case ModParameters::Release:
			env.SetParameter(EnvParameters::Release, value);
			return;
		case ModParameters::Shape:
			shape = (LfoShape)(int)(value * 0.9999 * (int)LfoShape::Count);
			return;
		case ModParameters::Sustain:
			env.SetParameter(EnvParameters::Sustain, value);
			return;
		case ModParameters::Sync:
			sync = value > 0.5;
			return;
		}
	}

	float Lfo::Process(int samples)
	{
		float envOut = env.Process(samples);
		float sample = GetSample();
		iterator += increment;
		output = envOut * sample;
		return output;
	}

	void Lfo::Reset()
	{
		env.Reset();
	}

	void Lfo::Update()
	{
		float samplesPerCycle = samplerate / freq;
		increment = (uint32_t)((1.0f / samplesPerCycle) * UINT32_MAX);
	}

	float Lfo::GetSample()
	{
		float ph = iterator / (float)UINT32_MAX;

		switch (shape)
		{
		case LfoShape::Pulse1:
			return ph < 0.2 ? 1.0 : -1.0;
		case LfoShape::Pulse2:
			return ph < 0.33333 ? 1.0 : -1.0;
		case LfoShape::Ramp:
			return -1.0 + 2 * ph;
		case LfoShape::SampleHold:
			if (iterator < updatedAt)
			{
				updatedAt = iterator;
				randIdx++;
				if (randIdx > AudioLib::Noise::NoiseSamples)
					randIdx -= AudioLib::Noise::NoiseSamples;

				return AudioLib::Noise::Random(randIdx);
			}
		case LfoShape::Saw:
			return 1.0 - 2 * ph;
		case LfoShape::Sine:
			return AudioLib::FastSin::Get(ph);
		case LfoShape::Square:
			return ph < 0.5 ? 1.0 : -1.0;
		case LfoShape::Triangle:
			if (ph < 0.25) return ph * 4.0;
			else if (ph < 0.75) return 1.0 - (ph - 0.25) * 4.0;
			else return (ph - 0.75) * 4.0;
		}
	}
}

