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

	void FilterMain::Process(float* input, int len)
	{
		if (!IsEnabled)
		{
			AudioLib::Utils::Copy(input, bypassBuffer, len);
			return;
		}

		if (type == 0)
			cascadeFilter.Process(input, len);
		else if (type == 1)
			zeroDelayFilter.Process(input, len);
	}

	float* FilterMain::GetOutput()
	{
		if (!IsEnabled)
			return bypassBuffer;

		if (type == 0)
			return cascadeFilter.GetOutput();
		else if (type == 1)
			return zeroDelayFilter.GetOutput();

		return 0;
	}

}