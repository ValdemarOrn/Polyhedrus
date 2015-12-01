#include "FilterHp.h"
#include "AudioLib/ValueTables.h"
#include "AudioLib/Utils.h"

namespace Leiftur
{
	FilterHp::FilterHp()
	{
		Cutoff = 0.0;
		Resonance = 0.0;
		CutoffMod = 0.0;
		updateCounter = 0;
		buffer = 0;
	}

	FilterHp::~FilterHp()
	{
		delete buffer;
	}

	void FilterHp::Initialize(int samplerate, int bufferSize, int modulationUpdateRate)
	{
		buffer = new float[bufferSize];
		cvToFreq.Initialize((float)samplerate);

		this->modulationUpdateRate = modulationUpdateRate;
		this->samplerate = samplerate;
		svf.Fs = samplerate * 3; // internal oversample by 3x
		svf.Nonlinear = false;
		Update();
	}

	void FilterHp::SetParameter(FilterHpParameters parameter, double value)
	{
		switch (parameter)
		{
		case FilterHpParameters::Cutoff:
			Cutoff = (float)value;
			break;
		case FilterHpParameters::Resonance:
			Resonance = (float)value;
			break;
		}
	}

	void FilterHp::Process(float* input, int len)
	{
		if (!IsEnabled)
		{
			AudioLib::Utils::Copy(input, buffer, len);
			return;
		}

		for (int i = 0; i < len; i++)
		{
			if (updateCounter <= 0)
			{
				Update();
				updateCounter = modulationUpdateRate;
			}

			// oversample by 3x
			float value = input[i];
			svf.ProcessLinear(value);
			svf.ProcessLinear(value);
			svf.ProcessLinear(value);
			buffer[i] = svf.Hp;
			updateCounter--;
		}
	}

	void FilterHp::Update()
	{
		float totalResonance = Resonance + ResonanceMod;
		totalResonance = AudioLib::Utils::Limit(totalResonance, 0.0f, 1.0f);
		totalResonance = (1 - AudioLib::ValueTables::Get((1 - totalResonance), AudioLib::ValueTables::Response2Oct)) * 0.95f;

		float voltage = 10.3f * Cutoff + CutoffMod;
		voltage = AudioLib::Utils::Limit(voltage, 0.0f, 10.0f);
		float fc = cvToFreq.GetFreqWarped(voltage);

		svf.Fc = fc;
		svf.Resonance = totalResonance;
		svf.Update();
	}

}

