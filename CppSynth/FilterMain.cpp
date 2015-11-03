#include "FilterMain.h"

namespace Leiftur
{
	FilterMain::FilterMain()
	{
		type = 0;
	}

	FilterMain::~FilterMain()
	{
	}

	void FilterMain::Initialize(int samplerate, int bufferSize, int modulationUpdateRate)
	{
		this->samplerate = samplerate;
		this->modulationUpdateRate = modulationUpdateRate;
		cascadeFilter.Initialize(samplerate, bufferSize, modulationUpdateRate);
		zeroDelayFilter.Initialize(samplerate, bufferSize, modulationUpdateRate);
	}

	void FilterMain::SetParameter(FilterMainParameters parameter, double value)
	{
		switch (parameter)
		{
		case FilterMainParameters::Cutoff:
			cascadeFilter.Cutoff = value;
			zeroDelayFilter.Cutoff = value;
			break;
		case FilterMainParameters::Drive:
			cascadeFilter.Drive = value;
			zeroDelayFilter.Drive = value;
			break;
		case FilterMainParameters::Resonance:
			cascadeFilter.Resonance = value;
			zeroDelayFilter.Resonance = value;
			break;
		case FilterMainParameters::Type:
			type = value;
			break;
		}
	}

	void FilterMain::Process(float * input, int len)
	{
		if (type == 0)
			cascadeFilter.Process(input, len);
		else if (type == 1)
			zeroDelayFilter.Process(input, len);
	}

	float* FilterMain::GetOutput()
	{
		if (type == 0)
			return cascadeFilter.GetOutput();
		else if (type == 1)
			return zeroDelayFilter.GetOutput();

		return 0;
	}

}