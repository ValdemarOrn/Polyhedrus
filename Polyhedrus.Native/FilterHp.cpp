#include "FilterHp.h"
#include "AudioLib/ValueTables.h"
#include "AudioLib/Utils.h"

namespace Polyhedrus
{
	FilterHp::FilterHp()
	{
		Cutoff = 0.0;
		Resonance = 0.0;
		CutoffMod = 0.0;
		buffer = 0;
	}

	FilterHp::~FilterHp()
	{
		delete buffer;
	}

	void FilterHp::Initialize(int samplerate, int bufferSize, int modulationUpdateRate)
	{
		buffer = new float[bufferSize]();
		cvToFreq.Initialize((float)samplerate);

		this->modulationUpdateRate = modulationUpdateRate;
		this->samplerate = samplerate;
		svf.Fs = (float)(samplerate * 2); // internal oversample by 2x
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

		Update();

		for (int i = 0; i < len; i++)
		{
			float value = input[i];
			// oversample by 2x
			svf.ProcessLinear(value);
			svf.ProcessLinear(value);
			buffer[i] = svf.Hp;
		}
	}

	std::vector<uint8_t> FilterHp::GetVisual(FilterHpParameters parameter, int* baseLevel)
	{
		std::vector<float> floatOutput;
		std::vector<uint8_t> output;
		float min = 9999;
		float max = -9999;

		/*if (parameter == FilterHpParameters::Keytrack)
		{
			*baseLevel = 128;
			min = 0;
			max = 255;

			for (int i = 0; i < 256; i++)
			{
				float value = i * Keytrack;
				floatOutput.push_back(i);
			}
		}
		else if (parameter == CharacterParameters::Decimate)
		{
			*baseLevel = 128;

			float decimate = Utils::Limit(Decimate, 0, 1);
			float factor = (float)std::pow(2, decimate * 4);
			float value = 0.0;
			float lastMod = 999;

			for (int i = 0; i < 256; i++)
			{
				float newMod = std::fmod(i, factor * 2);
				if (newMod < lastMod)
					value = std::sin(i / 256.0 * 2 * M_PI);
				lastMod = newMod;

				if (value < min) min = value;
				if (value > max) max = value;
				floatOutput.push_back(value);
			}
		}
		else if (parameter == CharacterParameters::Reduce)
		{
			*baseLevel = 128;

			float reduce = Utils::Limit(Reduce, 0, 1);
			float bits = std::pow(2, (1 - reduce) * 4);
			float factor = std::pow(2, bits - 1);
			float factorInv = 1.0f / factor;

			for (int i = 0; i < 256; i++)
			{
				float val = std::sin(i / 256.0 * 2 * M_PI);
				float value = ((int)(val * factor)) * factorInv;

				if (value < min) min = value;
				if (value > max) max = value;
				floatOutput.push_back(value);
			}
		}
		else if (parameter == CharacterParameters::Clip)
		{
			*baseLevel = 128;

			float factor = Utils::Limit(Clip, 0.0f, 0.9f) * 10 + 1.0f;

			for (int i = 0; i < 256; i++)
			{
				float value = std::sin(i / 256.0 * 2 * M_PI) * factor;
				value = Utils::Limit(value, -1.0f, 1.0f);

				if (value < min) min = value;
				if (value > max) max = value;
				floatOutput.push_back(value);
			}
		}

		// convert floats to byte values
		float scaler = 1.0 / (max - min);
		for (int i = 0; i < floatOutput.size(); i++)
		{
			float val = (floatOutput[i] - min) * scaler;
			output.push_back(val * 255.99);
		}*/

		return output;
	}

	std::vector<uint8_t> FilterHp::GetKeytrackVisual(double keytrack)
	{
		std::vector<uint8_t> output;

		for (int i = 0; i < 256; i++)
		{
			float val = (float)((-1 + 2 * i / 256.0) * keytrack);
			int iVal = 128 + (int)(val * 128);
			if (iVal < 0) iVal = 0;
			if (iVal > 255) iVal = 255;
			output.push_back(iVal);
		}
		
		return output;
	}

	void FilterHp::Update()
	{
		float totalResonance = Resonance + ResonanceMod;
		totalResonance = AudioLib::Utils::Limit(totalResonance, 0.0f, 1.0f);
		totalResonance = ((float)(1 - AudioLib::ValueTables::Get((1 - totalResonance), AudioLib::ValueTables::Response2Oct))) * 0.95f;

		float voltage = Cutoff + CutoffMod;
		voltage = AudioLib::Utils::Limit(voltage, 0.0f, 9.5f);
		float fc = cvToFreq.GetFreqWarped(voltage);

		svf.Fc = fc;
		svf.Resonance = totalResonance;
		svf.Update();
	}

}

