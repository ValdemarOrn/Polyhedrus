#include "FilterDualSvf.h"
#include "AudioLib/ValueTables.h"
#include "AudioLib/Utils.h"

namespace Leiftur
{
	FilterDualSvf::FilterDualSvf()
	{
		Drive = 0;
		Cutoff = 0;
		Resonance = 0;
		CutoffMod = 0;
		ResonanceMod = 0;
		DriveMod = 0;
		buffer = 0;
		gain = 1;
		gainInv = 1;
		totalResonance = 0;
	}

	FilterDualSvf::~FilterDualSvf()
	{
		delete buffer;
	}

	void FilterDualSvf::Initialize(int samplerate, int bufferSize, int modulationUpdateRate)
	{
		buffer = new float[bufferSize];
		this->modulationUpdateRate = modulationUpdateRate;
		this->samplerate = samplerate;
		fsinv = 1.0f / (Oversample * samplerate);
		cvToFreq.Initialize((float)samplerate);
		svf1.Fs = samplerate * 2; // internal oversample by 2x
		svf2.Fs = samplerate * 2;

		Cutoff = 1;
		updateCounter = 0;
		Update();
	}

	void FilterDualSvf::Process(float* input, int len)
	{
		for (int i = 0; i < len; i++)
		{
			if (updateCounter <= 0)
			{
				Update();
				updateCounter = modulationUpdateRate;
			}

			float val = input[i] * gain;
			
			// oversample by 2x
			svf1.Process(val);
			svf1.Process(val);
			val = svf1.Lp;

			// oversample by 2x
			svf2.Process(val);
			svf2.Process(val);
			val = svf2.Lp;

			buffer[i] = val * gainInv;
			updateCounter--;
		}
	}

	void FilterDualSvf::Update()
	{
		float driveTotal = Drive + DriveMod;
		driveTotal = AudioLib::Utils::Limit(driveTotal, 0.0f, 1.0f);

		gain = (0.1f + 2.0f * driveTotal * driveTotal);
		gainInv = gain < 1.0 ? std::sqrt(1.0f / gain) : 1.0f;
		gainInv *= 2; // gain fudge;

		totalResonance = Resonance + ResonanceMod;
		totalResonance = AudioLib::Utils::Limit(totalResonance, 0.0f, 1.0f);
		totalResonance = (1 - AudioLib::ValueTables::Get((1 - totalResonance), AudioLib::ValueTables::Response2Oct)) * 0.95f;

		float voltage = 10.1f * Cutoff + CutoffMod;
		voltage = AudioLib::Utils::Limit(voltage, 0.0f, 10.1f);
		float fc = cvToFreq.GetFreqWarped(voltage);

		svf1.Fc = fc;
		svf2.Fc = fc;
		svf1.Resonance = totalResonance;
		svf2.Resonance = totalResonance;
		svf1.Update();
		svf2.Update();
	}
}

