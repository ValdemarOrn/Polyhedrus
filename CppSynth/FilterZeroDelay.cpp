#define _USE_MATH_DEFINES

#include "FilterZeroDelay.h"
#include "AudioLib/ValueTables.h"
#include "AudioLib/Utils.h"

namespace Leiftur
{
	FilterZeroDelay::FilterZeroDelay()
	{
		Drive = 0.0;
		Cutoff = 1.0;
		Resonance = 0.0;
		ResonanceMod = 0.0;
		CutoffMod = 0.0;
		DriveMod = 0.0;
		buffer = 0;
		driveTotal = 0.0;
		feedback = 0.0;
	}

	FilterZeroDelay::~FilterZeroDelay()
	{
		delete buffer;
	}

	void FilterZeroDelay::Initialize(int samplerate, int bufferSize, int modulationUpdateRate)
	{
		buffer = new float[bufferSize];
		this->modulationUpdateRate = modulationUpdateRate;
		this->samplerate = samplerate;
		T = 1.0f / (Oversample * samplerate);

		Cutoff = 1;
		updateCounter = 0;
		Update();
	}

	float* FilterZeroDelay::GetOutput()
	{
		return buffer;
	}

	void FilterZeroDelay::Process(float* input, int len)
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

			float x = ProcessSample(input[i]);
			buffer[i] = x * gInv;
			updateCounter--;
		}
	}

	float FilterZeroDelay::ProcessSample(float input)
	{
		float x = 0.0;
		
		for (int i = 0; i < Oversample; i++)
		{
			auto fb = totalResonance * 5.5 * feedback;
			x = AudioLib::Utils::TanhPoly(input * gain - fb);

			x = lp1.Process(x);
			x = lp2.Process(x);			x = lp3.Process(x);			x = lp4.Process(x);			feedback = AudioLib::Utils::TanhPoly(x);
		}

		return x;
	}

	void FilterZeroDelay::Update()
	{
		driveTotal = Drive + DriveMod;
		driveTotal = AudioLib::Utils::Limit(driveTotal, 0.0f, 1.0f);

		gain = (0.5f + 1.5f * driveTotal * driveTotal);

		// Voltage is 1V/OCt, C0 = 16.3516Hz
		float voltage = 10.3 * Cutoff + CutoffMod;
		voltage = AudioLib::Utils::Limit(voltage, 0.0f, 10.3f);

		float freq = FilterZeroDelay::GetCvFreq(voltage);
		float omegaCWarped = freq * 2 * M_PI;
		float omegaC = 2 / T * std::tan(omegaCWarped * T / 2);

		totalResonance = Resonance + ResonanceMod;
		totalResonance = totalResonance * std::sqrt(8000 / (freq + 8000)); // fudge factor 
		totalResonance = AudioLib::Utils::Limit(totalResonance, 0.0f, 0.999f);

		float g = omegaC * T / 2;
		lp1.g = g;
		lp2.g = g;
		lp3.g = g;
		lp4.g = g;
	}

}
