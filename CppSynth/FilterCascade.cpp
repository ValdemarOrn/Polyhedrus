#include "FilterCascade.h"
#include "AudioLib/ValueTables.h"
#include "AudioLib/Utils.h"

namespace Polyhedrus
{
	float FilterCascade::CVtoAlpha[CVtoAlphaSize];

	void FilterCascade::ComputeCVtoAlpha(int samplerate)
	{
		double fsInv = 1.0 / samplerate;
		for (int i = 0; i < CVtoAlphaSize; i++)
		{
			double freq = 440.0 * std::pow(2, (i*0.01 - 69.0 + 12) / 12);
			CVtoAlpha[i] = (float)((1.0f - 2.0f * freq * fsInv) * (1.0f - 2.0f * freq * fsInv));
		}
	}

	FilterCascade::FilterCascade()
	{
		Drive = 0.0;
		Cutoff = 1.0;
		Resonance = 0.0;
		ResonanceMod = 0.0;
		CutoffMod = 0.0;
		DriveMod = 0.0;
		buffer = 0;
		SetMode(InternalFilterMode::Lp24);
	}

	FilterCascade::~FilterCascade()
	{
		delete buffer;
	}

	void FilterCascade::Initialize(int samplerate, int bufferSize, int modulationUpdateRate)
	{
		ComputeCVtoAlpha(samplerate);
		buffer = new float[bufferSize];
		this->modulationUpdateRate = modulationUpdateRate;
		this->samplerate = samplerate;
		fsinv = 1.0f / (Oversample * samplerate);

		Cutoff = 1;
		updateCounter = 0;
		oversampledInput = 0;
		Update();
	}

	void FilterCascade::Process(float * input, int len)
	{
		float gInv = sqrt(1.0f / gain);

		for (int i = 0; i < len; i++)
		{
			if (updateCounter <= 0)
			{
				Update();
				gInv = sqrt(1.0f / gain);
				updateCounter = modulationUpdateRate;
			}

			float value = ProcessSample(input[i]) * gInv;
			buffer[i] = value;
			updateCounter--;
		}
	}

	float FilterCascade::ProcessSample(float input)
	{
		input *= gain;
		float mx = 1.0f / Oversample;
		float sum = 0.0;

		for (int i = 0; i < Oversample; i++)
		{
			float in = mx * (i * input + (Oversample - i) * oversampledInput); // linear interpolation
			in = AudioLib::Utils::TanhPoly(in);

			float fb = totalResonance * 4.2f * (feedback - 0.5f * in);
			float val = in - fb;
			x = val;

			// 4 cascaded low pass stages
			a = (1 - p) * val + p * a;
			val = a;
			b = (1 - p) * val + p * b;
			val = b;
			c = (1 - p) * val + p * c;
			val = c;
			d = (1 - p) * val + p * d;
			val = d;

			feedback = AudioLib::Utils::TanhPoly(val);
		}

		oversampledInput = input;
		float sample = (c0 * x + c1 * a + c2 * b + c3 * c + c4 * d) * (1 - totalResonance * 0.5f);
		return sample * 4; // gain fudge
	}

	void FilterCascade::SetMode(InternalFilterMode mode)
	{
		if (mode < InternalFilterMode::Lp24 || mode >= InternalFilterMode::Count)
			return;

		switch (mode)
		{
		case InternalFilterMode::Lp6:
			c0 = 0.0; c1 = 1.0; c2 = 0.0; c3 = 0.0; c4 = 0.0;
			break;
		case InternalFilterMode::Lp12:
			c0 = 0.0; c1 = 0.0; c2 = 1.0; c3 = 0.0; c4 = 0.0;
			break;
		case InternalFilterMode::Lp18:
			c0 = 0.0; c1 = 0.0; c2 = 0.0; c3 = 1.0; c4 = 0.0;
			break;
		case InternalFilterMode::Lp24:
			c0 = 0.0; c1 = 0.0; c2 = 0.0; c3 = 0.0; c4 = 1.0;
			break;
		case InternalFilterMode::Hp6:
			c0 = 1.0; c1 = -1.0; c2 = 0.0; c3 = 0.0; c4 = 0.0;
			break;
		case InternalFilterMode::Hp12:
			c0 = 1.0; c1 = -2.0; c2 = 1.0; c3 = 0.0; c4 = 0.0;
			break;
		case InternalFilterMode::Hp18:
			c0 = 1.0; c1 = -3.0; c2 = 3.0; c3 = -1.0; c4 = 0.0;
			break;
		case InternalFilterMode::Hp24:
			c0 = 1.0; c1 = -4.0; c2 = 6.0; c3 = -4.0; c4 = 1.0;
			break;
		case InternalFilterMode::Bp12_12:
			c0 = 0.0; c1 = 0.0; c2 = 1.0; c3 = -2.0; c4 = 1.0;
			break;
		case InternalFilterMode::Bp6_18:
			c0 = 0.0; c1 = 0.0; c2 = 0.0; c3 = 1.0; c4 = -1.0;
			break;
		case InternalFilterMode::Bp18_6:
			c0 = 0.0; c1 = 1.0; c2 = -3.0; c3 = 3.0; c4 = -1.0;
			break;
		case InternalFilterMode::Bp6_12:
			c0 = 0.0; c1 = 0.0; c2 = 1.0; c3 = -1.0; c4 = 0.0;
			break;
		case InternalFilterMode::Bp12_6:
			c0 = 0.0; c1 = 1.0; c2 = -2.0; c3 = 1.0; c4 = 0.0;
			break;
		case InternalFilterMode::Bp6_6:
			c0 = 0.0; c1 = 1.0; c2 = -1.0; c3 = 0.0; c4 = 0.0;
			break;
		}
	}

	void FilterCascade::Update()
	{
		float driveTotal = Drive + DriveMod;
		driveTotal = AudioLib::Utils::Limit(driveTotal, 0.0f, 1.0f);

		gain = (0.1f + 2.0f * driveTotal * driveTotal);

		totalResonance = Resonance + ResonanceMod;
		totalResonance = AudioLib::Utils::Limit(totalResonance, 0.0f, 1.0f);
		totalResonance = AudioLib::ValueTables::Get(totalResonance, AudioLib::ValueTables::Response2Oct) * 0.999f;

		// Voltage is 1V/OCt, C0 = 16.3516Hz
		float voltage = Cutoff + CutoffMod;
		voltage = AudioLib::Utils::Limit(voltage, 0.0f, 10.3f);

		p = CVtoAlpha[(int)(voltage * 1000.0)];
	}

}
