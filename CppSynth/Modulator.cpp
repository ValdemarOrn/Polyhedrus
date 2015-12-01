#include "Modulator.h"
#include "AudioLib/Noise.h"
#include "AudioLib/FastSin.h"
#include "AudioLib/Utils.h"

using namespace AudioLib;

namespace Leiftur
{
	Modulator::Modulator()
	{
		Delay = 0.0f;
		Attack = 0.0f;
		Hold = 0.0f;
		Decay = 0.0f;
		Sustain = 1.0f;
		Release = 1.0f;

		DelayMod = 0.0f;
		AttackMod = 0.0f;
		HoldMod = 0.0f;
		DecayMod = 0.0f;
		SustainMod = 0.0f;
		ReleaseMod = 0.0f;

		Shape = LfoShape::Triangle;

		Phase = 0.0f;
		Freq = 0.5f;
		Slew = 0.0f;
		Steps = 1;
		Sync = false;

		FreqMod = 0.0f;
		SlewMod = 0.0f;
		StepsMod = 0.0f;

		samplerate = 48000;
		Output = 0.0f;
		EnvOutput = 0.0f;
		prevLfoSample = 0.0f;
		slewTotal = 0.0f;

		iterator = 0;
		increment = 1;
		gate = false;

		prevIterator = UINT32_MAX;

		env.SetParameter(EnvParameters::AttackCurve, 0.0);
		env.SetParameter(EnvParameters::DecayCurve, 0.0);
		env.SetParameter(EnvParameters::ReleaseCurve, 0.0);
		Update();
	}

	Modulator::~Modulator()
	{
	}

	void Modulator::Initialize(int samplerate)
	{
		this->samplerate = samplerate;
		env.Initialize(samplerate);
	}

	void Modulator::SetParameter(ModParameters parameter, double value)
	{
		switch (parameter)
		{
		case ModParameters::Delay:
			Delay = (float)value;
			return;
		case ModParameters::Attack:
			Attack = (float)value;
			return;
		case ModParameters::Hold:
			Hold = (float)value;
			return;
		case ModParameters::Decay:
			Decay = (float)value;
			return;
		case ModParameters::Sustain:
			Sustain = (float)value;
			return;
		case ModParameters::Release:
			Release = (float)value;
			return;

		case ModParameters::Phase:
			Phase = (float)value;
			return;
		case ModParameters::Freq:
			Freq = (float)value;
			return;
		case ModParameters::Shape:
			Shape = (LfoShape)Parameters::FloorToInt(value);
			return;
		case ModParameters::Slew:
			Slew = (float)value;
			return;
		case ModParameters::Steps:
			Steps = value;
			return;
		case ModParameters::Sync:
			Sync = value >= 0.5;
			return;

		case ModParameters::AttackCurve:
			env.SetParameter(EnvParameters::AttackCurve, value);
			return;
		case ModParameters::DecayCurve:
			env.SetParameter(EnvParameters::AttackCurve, value);
			return;
		case ModParameters::ReleaseCurve:
			env.SetParameter(EnvParameters::AttackCurve, value);
			return;
		case ModParameters::Retrigger:
			env.SetParameter(EnvParameters::Retrigger, value);
			break;
		}
	}

	float Modulator::Process(int samples)
	{
		Update();
		
		float sample = GetSample();
		float totalSlew = slewPerSample * samples;
		if (sample > prevLfoSample) // going up
		{
			if (sample - prevLfoSample > totalSlew)
				sample = prevLfoSample + totalSlew;
		}
		else // going down
		{
			if (prevLfoSample - sample > totalSlew)
				sample = prevLfoSample - totalSlew;
		}

		float envOut = env.Process(samples);
		prevIterator = iterator;
		iterator += increment * samples;
		EnvOutput = envOut;
		float quantizedSample = std::ceil(sample * stepCount) / stepCount;
		Output = envOut * quantizedSample;

		prevLfoSample = sample;
		return Output;
	}

	void Modulator::Reset()
	{
		if (Phase < 0.999)
		{
			iterator = (uint32_t)(((double)UINT32_MAX) * ((double)Phase));
			prevLfoSample = GetSample();
			prevIterator = UINT32_MAX;
		}
	}

	void Modulator::Update()
	{
		float freq = GetFrequency(Utils::Limit(Freq + FreqMod, 0.0f, 1.0f));
		env.SetParameter(EnvParameters::Attack, Utils::Limit(Attack + AttackMod, 0.0f, 1.0f));
		env.SetParameter(EnvParameters::Hold, Utils::Limit(Hold + HoldMod, 0.0f, 1.0f));
		env.SetParameter(EnvParameters::Decay, Utils::Limit(Decay + DecayMod, 0.0f, 1.0f));
		env.SetParameter(EnvParameters::Sustain, Utils::Limit(Sustain + SustainMod, 0.0f, 1.0f));
		env.SetParameter(EnvParameters::Release, Utils::Limit(Release + ReleaseMod, 0.0f, 1.0f));
		float samplesPerCycle = samplerate / freq;
		increment = (uint32_t)((1.0f / samplesPerCycle) * UINT32_MAX);


		slewTotal = 1 - Utils::Limit(Slew + SlewMod, 0.0f, 1.0f);
		// Todo: Convert to lookup table
		double slewVSec = std::pow(2, 12 * slewTotal) / 16.0;
		slewPerSample = (float)(slewVSec / samplerate);


		stepCount = (int)(ValueTables::Get(Utils::Limit(Steps + StepsMod, 0.0f, 1.0f), ValueTables::Response3Oct) * 256);
		if (stepCount == 256) stepCount = 100000;
	}

	float Modulator::GetSample()
	{
		float ph = iterator / (float)UINT32_MAX;

		switch (Shape)
		{
		case LfoShape::Pulse1:
			return ph < 0.2 ? 1.0f : -1.0f;
		case LfoShape::Pulse2:
			return ph < 0.33333 ? 1.0f : -1.0f;
		case LfoShape::Ramp:
			return -1.0f + 2.0f * ph;
		case LfoShape::SampleHold:
			if (iterator < prevIterator) // overflow
				currentSh = random.NextFloat();
			return currentSh;
		case LfoShape::Saw:
			return 1.0f - 2 * ph;
		case LfoShape::Sine:
			return (float)AudioLib::FastSin::Get(ph);
		case LfoShape::Square:
			return ph < 0.5 ? 1.0f : -1.0f;
		case LfoShape::Triangle:
			if (ph < 0.25) return ph * 4.0f;
			else if (ph < 0.75) return 1.0f - (ph - 0.25f) * 4.0f;
			else return -1 + (ph - 0.75f) * 4.0f;
		default:
			return 0.0f;
		}
	}
}

