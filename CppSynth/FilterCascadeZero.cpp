#include "FilterCascadeZero.h"
#include "AudioLib/ValueTables.h"
#include "AudioLib/Utils.h"
#include "AudioLib/MathDefs.h"

namespace Leiftur
{
	float FilterCascadeZero::CVtoAlpha[CVtoAlphaSize];

	void FilterCascadeZero::ComputeCVtoAlpha(int samplerate)
	{
		double fsInv = 1.0 / samplerate;
		for (int i = 0; i < CVtoAlphaSize; i++)
		{
			double freq = 440.0 * std::pow(2, (i*0.01 - 69.0 + 12) / 12);
			CVtoAlpha[i] = (float)((1.0f - 2.0f * freq * fsInv) * (1.0f - 2.0f * freq * fsInv));
		}
	}

	FilterCascadeZero::FilterCascadeZero()
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

	FilterCascadeZero::~FilterCascadeZero()
	{
		delete buffer;
	}

	void FilterCascadeZero::Initialize(int samplerate, int bufferSize, int modulationUpdateRate)
	{
		ComputeCVtoAlpha(samplerate);
		buffer = new float[bufferSize];
		this->modulationUpdateRate = modulationUpdateRate;
		this->samplerate = samplerate;
		fsinv = 1.0f / (Oversample * samplerate);
		cvToFreq.Initialize((float)samplerate);

		Cutoff = 1;
		updateCounter = 0;
		oversampledInput = 0;
		Update();
	}

	void FilterCascadeZero::Process(float * input, int len)
	{
		for (int i = 0; i < len; i++)
		{
			if (updateCounter <= 0)
			{
				Update();
				updateCounter = modulationUpdateRate;
			}

			float value = ProcessSample(input[i]) * gainInv;
			buffer[i] = value;
			updateCounter--;
		}
	}

	float* FilterCascadeZero::GetOutput()
	{
		return buffer;
	}

	float FilterCascadeZero::ProcessSample(float input)
	{
		input *= gain;
		float mx = 1.0f / Oversample;
		float output = 0.0;

		for (int i = 0; i < Oversample; i++)
		{
			float in = mx * (i * input + (Oversample - i) * oversampledInput); // linear interpolation
			
			float fb = totalResonance * 4.8f * (feedback - 0.5f * in);
			float value = in - fb;
			
			// 4 cascaded low pass stages
			value = lp1.Process(value);
			value = AudioLib::Utils::TanhLookup(value);
			value = lp2.Process(value);
			value = AudioLib::Utils::TanhLookup(value);
			value = lp3.Process(value);
			value = AudioLib::Utils::TanhLookup(value);
			value = lp4.Process(value);
			value = AudioLib::Utils::TanhLookup(value);
			feedback = value;
			output = feedback;
		}

		oversampledInput = input;
		float sample = output * (1 - totalResonance * 0.5f);
		return sample;
	}

	void FilterCascadeZero::Update()
	{
		driveTotal = Drive + DriveMod;
		driveTotal = AudioLib::Utils::Limit(driveTotal, 0.0f, 1.0f);

		gain = (0.2f + 1.8f * driveTotal * driveTotal);
		gainInv = gain < 1.0 ? std::sqrt(1.0 / gain) : 1.0;

		// Voltage is 1V/OCt, C0 = 16.3516Hz
		float voltage = 10.3 * Cutoff + CutoffMod;
		voltage = AudioLib::Utils::Limit(voltage, 0.0f, 10.3f);
		float fc = cvToFreq.GetFreqWarped(voltage);

		totalResonance = Resonance + ResonanceMod;
		totalResonance = totalResonance * std::sqrt(12000 / (fc + 12000)); // fudge factor 
		totalResonance = AudioLib::Utils::Limit(totalResonance, 0.0f, 0.999f);

		float g = fc * M_PI * fsinv;
		lp1.SetG(g);
		lp2.SetG(g);
		lp3.SetG(g);
		lp4.SetG(g);
	}

}
