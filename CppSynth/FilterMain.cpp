#include "FilterMain.h"
#include "AudioLib/Utils.h"

namespace Leiftur
{
	FilterMain::FilterMain()
	{
		type = 0;
		bypassBuffer = 0;
		IsEnabled = true;
	}

	FilterMain::~FilterMain()
	{
		delete bypassBuffer;
	}

	void FilterMain::Initialize(int samplerate, int bufferSize, int modulationUpdateRate)
	{
		this->bypassBuffer = new float[bufferSize];
		this->samplerate = samplerate;
		this->modulationUpdateRate = modulationUpdateRate;
		cascadeFilter.Initialize(samplerate, bufferSize, modulationUpdateRate);
		cascadeZeroFilter.Initialize(samplerate, bufferSize, modulationUpdateRate);
		svfFilter.Initialize(samplerate, bufferSize, modulationUpdateRate);
		hp.SetFc(10.0 / 24000.0);
	}

	void FilterMain::SetParameter(FilterMainParameters parameter, double value)
	{
		switch (parameter)
		{
		case FilterMainParameters::Cutoff:
			cascadeFilter.Cutoff = (float)value;
			cascadeZeroFilter.Cutoff = (float)value;
			svfFilter.Cutoff = (float)value;
			break;
		case FilterMainParameters::Drive:
			cascadeFilter.Drive = (float)value;
			cascadeZeroFilter.Drive = (float)value;
			svfFilter.Drive = (float)value;
			break;
		case FilterMainParameters::Resonance:
			cascadeFilter.Resonance = (float)value;
			cascadeZeroFilter.Resonance = (float)value;
			svfFilter.Resonance = (float)value;
			break;
		case FilterMainParameters::Mode:
			cascadeFilter.SetMode((InternalFilterMode)Parameters::FloorToInt(value * ((int)InternalFilterMode::Count - 0.00001)));
			svfFilter.Mode = (float)value;
			break;
		case FilterMainParameters::Type:
			type = Parameters::FloorToInt(value);
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

		if (type == 0)
			svfFilter.Process(input, len);
		else if (type == 1)
			cascadeZeroFilter.Process(input, len);
		else //if (type == 2)
			cascadeFilter.Process(input, len);			
	}

	float* FilterMain::GetOutput()
	{
		if (!IsEnabled)
			return bypassBuffer;

		if (type == 0)
			return svfFilter.GetOutput();
		else if (type == 1)
			return cascadeZeroFilter.GetOutput();
		else// if (type == 2)
			return cascadeFilter.GetOutput();

		return 0;
	}

}