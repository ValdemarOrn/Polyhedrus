#include "FilterMain.h"

namespace Leiftur
{
	FilterMain::FilterMain()
	{
	}

	FilterMain::~FilterMain()
	{
	}

	void FilterMain::Initialize(int samplerate, int bufferSize, int modulationUpdateRate)
	{
		this->samplerate = samplerate;
		this->modulationUpdateRate = modulationUpdateRate;
		cascadeFilter.Initialize(samplerate, bufferSize, modulationUpdateRate);
	}

	void FilterMain::SetParameter(FilterMainParameters parameter, double value)
	{
		switch (parameter)
		{
		case FilterMainParameters::Cutoff:
			cascadeFilter.Cutoff = value;
			break;
		case FilterMainParameters::Drive:
			cascadeFilter.Drive = value;
			break;
		case FilterMainParameters::Resonance:
			cascadeFilter.Resonance = value;
			break;
		case FilterMainParameters::Type:
			// Todo
			break;
		}
	}

	void FilterMain::Process(float * input, int len)
	{
		cascadeFilter.Process(input, len);
	}

	float * FilterMain::GetOutput()
	{
		return cascadeFilter.GetOutput();
	}

	void FilterMain::Update()
	{
	}

}