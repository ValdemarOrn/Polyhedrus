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

	void FilterZeroDelay::Initialize(int samplerate, int oversampling, int bufferSize, int modulationUpdateRate)
	{
		this->oversample = oversampling;
		this->oversampleInv = 1.0f / oversample;
		buffer = new float[bufferSize];
		cvToFreq.Initialize(samplerate);
		this->modulationUpdateRate = modulationUpdateRate;
		this->samplerate = samplerate;
		T = 1.0f / (oversample * samplerate);

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
		input *= gain;
		float value = 0.0;
		float output = 0.0;
		
		for (int i = 0; i < oversample; i++)
		{
			//input = AudioLib::Utils::TanhPoly(input);
			auto fb = totalResonance * 5.5 * (feedback - 0.5 * input);
			value = AudioLib::Utils::TanhLookup(input - fb);

			value = lp1.Process(value);
			value = lp2.Process(value);
			value = lp3.Process(value);
			value = lp4.Process(value);
			feedback = AudioLib::Utils::TanhLookup(value);
			output += feedback;
		}

		return output * oversampleInv;
	}

	void FilterZeroDelay::Update()
	{
		driveTotal = Drive + DriveMod;
		driveTotal = AudioLib::Utils::Limit(driveTotal, 0.0f, 1.0f);

		gain = (0.5f + 2.5f * driveTotal * driveTotal);

		// Voltage is 1V/OCt, C0 = 0V = 16.3516Hz
		float voltage = 10.3 * Cutoff + CutoffMod;
		float fc = cvToFreq.GetFreqWarped(voltage);

		totalResonance = Resonance + ResonanceMod;
		totalResonance = totalResonance * std::sqrt(8000 / (fc + 8000)); // fudge factor 
		totalResonance = AudioLib::Utils::Limit(totalResonance, 0.0f, 0.999f);

		//float g = omegaC * T / 2;
		float g = fc * M_PI * T;
		lp1.SetG(g);
		lp2.SetG(g);
		lp3.SetG(g);
		lp4.SetG(g);
	}

}
