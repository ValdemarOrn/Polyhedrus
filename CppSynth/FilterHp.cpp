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

		this->modulationUpdateRate = modulationUpdateRate;
		this->samplerate = samplerate;
		biquad.Frequency = 10;
		biquad.SetQ(0.5);
		biquad.SetSamplerate(samplerate);
		biquad.Type = AudioLib::Biquad::FilterType::HighPass;
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
			biquad.SetQ(0.5 + value * 10);
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

			float value = (float)biquad.Process(input[i]);
			buffer[i] = value;
			updateCounter--;
		}
	}

	float * FilterHp::GetOutput()
	{
		return buffer;
	}

	void FilterHp::Update()
	{
		float value = Cutoff + CutoffMod;
		value = AudioLib::Utils::Limit(value, 0.0, 1.0);

		biquad.Frequency = 10 + AudioLib::ValueTables::Get(value, AudioLib::ValueTables::Response3Dec) * 19000;
		biquad.Update();
	}

}

