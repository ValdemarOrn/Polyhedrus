#include "FilterCascade.h"
#include "AudioLib/ValueTables.h"
#include "AudioLib/Utils.h"
#include <cmath>

namespace Leiftur
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
		driveTotal = 0.0;
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
		VD = 1;
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

	float* FilterCascade::GetOutput()
	{
		return buffer;
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

			float fb = totalResonance * 5.0f * (feedback - 0.5f * in);
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
		float sample = (VX * x + VA * a + VB * b + VC * c + VD * d) * (1 - totalResonance * 0.5f);
		return sample;
	}

	void FilterCascade::Update()
	{
		driveTotal = Drive + DriveMod;
		driveTotal = AudioLib::Utils::Limit(driveTotal, 0.0f, 1.0f);

		gain = (0.1f + 1.0f * driveTotal * driveTotal);

		totalResonance = Resonance + ResonanceMod;
		totalResonance = AudioLib::Utils::Limit(totalResonance, 0.0f, 0.999f);

		// Voltage is from C0 = 16.3516Hz to C10 = 16744.04Hz
		float voltage = 10 * Cutoff + CutoffMod;
		voltage = AudioLib::Utils::Limit(voltage, 0.0, 9.9999);

		p = CVtoAlpha[(int)(voltage * 1000.0)];
	}

}
