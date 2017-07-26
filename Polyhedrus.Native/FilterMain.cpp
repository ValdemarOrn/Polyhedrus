#include "FilterMain.h"
#include "AudioLib/Utils.h"
#include "AudioLib/Biquad.h"

namespace Polyhedrus
{
	FilterMain::FilterMain()
	{
		type = FilterMainType::TrueZero;
		bypassBuffer = 0;
		IsEnabled = true;
	}

	FilterMain::~FilterMain()
	{
		delete bypassBuffer;
	}

	void FilterMain::Initialize(int samplerate, int bufferSize, int modulationUpdateRate)
	{
		this->bypassBuffer = new float[bufferSize]();
		this->samplerate = samplerate;
		this->modulationUpdateRate = modulationUpdateRate;
		cascadeFilter.Initialize(samplerate, bufferSize, modulationUpdateRate);
		cascadeZeroFilter.Initialize(samplerate, bufferSize, modulationUpdateRate);
		svfFilter.Initialize(samplerate, bufferSize, modulationUpdateRate);
		trueZeroFilter.Initialize(samplerate, bufferSize, modulationUpdateRate);
		hp.SetFc(10.0f / 24000.0f);
	}

	void FilterMain::SetParameter(FilterMainParameters parameter, double value)
	{
		switch (parameter)
		{
		case FilterMainParameters::Cutoff:
			cutoff = cascadeZeroFilter.CvToFreq.GetFreqWarped(AudioLib::Utils::Limit((float)value, 0.0f, 10.3f));
			cascadeFilter.Cutoff = (float)value;
			cascadeZeroFilter.Cutoff = (float)value;
			svfFilter.Cutoff = (float)value;
			trueZeroFilter.Cutoff = (float)value;
			break;
		case FilterMainParameters::Drive:
			drive = (float)value;
			cascadeFilter.Drive = (float)value;
			cascadeZeroFilter.Drive = (float)value;
			svfFilter.Drive = (float)value;
			trueZeroFilter.Drive = (float)value;
			break;
		case FilterMainParameters::Resonance:
			resonance = (float)value;
			cascadeFilter.Resonance = (float)value;
			cascadeZeroFilter.Resonance = (float)value;
			svfFilter.Resonance = (float)value;
			trueZeroFilter.Resonance = (float)value;
			break;
		case FilterMainParameters::Mode:
			cascadeFilter.SetMode((InternalFilterMode)Parameters::FloorToInt(value * ((int)InternalFilterMode::Count - 0.00001)));
			svfFilter.Mode = (float)value;
			break;
		case FilterMainParameters::Type:
			type = (FilterMainType)Parameters::FloorToInt(value);
			break;
		}
	}

	void FilterMain::Process(float* input, int len)
	{
		if (!IsEnabled)
		{
			AudioLib::Utils::Copy(input, bypassBuffer, len);
			return;
		}
		else
		{
			for (int i = 0; i < len; i++)
			{
				input[i] = hp.Process(input[i]);
			}
		}

		

		if (type == FilterMainType::TrueZero)
			trueZeroFilter.Process(input, len);
		else if (type == FilterMainType::DualSvf)
			svfFilter.Process(input, len);
		/*
			cascadeZeroFilter.Process(input, len);
		else //if (type == 2)
			cascadeFilter.Process(input, len);	*/		
	}

	float* FilterMain::GetOutput()
	{
		if (!IsEnabled)
			return bypassBuffer;

		if (type == FilterMainType::TrueZero)
			return trueZeroFilter.GetOutput();
		else if (type == FilterMainType::DualSvf)
			return svfFilter.GetOutput();
		/*else// if (type == 2)
			return cascadeFilter.GetOutput();
		*/
		return 0;
	}

	std::vector<uint8_t> FilterMain::GetVisual()
	{
		auto transform = [](std::vector<float> input) ->std::vector<uint8_t>
		{
			std::vector<uint8_t> output;
			float min = -60;
			float max = 80;
			for (size_t i = 0; i < input.size(); i++)
			{
				float db = (float)AudioLib::Utils::Gain2DB(input[i]);
				if (db < min) db = min;
				if (db > max) db = max;

				int val = (int)((db - min) * 1.8f);
				output.push_back(val);
			}

			return output;
		};

		if (type == FilterMainType::DualSvf)
		{
			auto vv = svfFilter.GetMagnitudeResponse();
			return transform(vv);
		}
		else
		{
			auto vv = AudioLib::Biquad::GetLowpassMagnitude(cutoff, resonance);
			return transform(vv);
		}
	}

	std::vector<uint8_t> FilterMain::GetDriveVisual()
	{
		std::vector<uint8_t> output;
		std::vector<float> floatOutput;
		float gain = (0.2f + 1.8f * drive * drive);
		float min = 9999.0f;
		float max = -9999.0f;

		for (size_t i = 0; i < 256; i++)
		{
			float value = (float)((-1 + 2 * i / 255.0) * gain);
			value = std::tanh(value);
			if (value < min) min = value;
			if (value > max) max = value;
			floatOutput.push_back(value);
		}

		float scaler = 1.0f / (max - min) * 255.99f;

		for (size_t i = 0; i < floatOutput.size(); i++)
		{
			output.push_back((uint8_t)((floatOutput[i] - min) * scaler));
		}

		return output;
	}

}