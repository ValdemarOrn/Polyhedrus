#include "Vca.h"

namespace Polyhedrus
{
	Vca::Vca()
	{
		ControlVoltage = 0.0f;
		currentCv = 0.0f;
		buffer = 0;
	}

	Vca::~Vca()
	{
		delete buffer;
	}

	void Vca::Initialize(int samplerate, int bufferSize, int modulationUpdateRate)
	{
		buffer = new float[bufferSize];
		this->modulationUpdateRate = modulationUpdateRate;
		this->samplerate = samplerate;
	}

	void Vca::Process(float* input, int len)
	{
		// smooth linear interpolation of CV
		float diff = (ControlVoltage - currentCv) / modulationUpdateRate;

		for (size_t i = 0; i < len; i++)
		{
			if (i < modulationUpdateRate)
				currentCv = currentCv + diff;

			buffer[i] = input[i] * currentCv;
		}
	}

	float* Vca::GetOutput()
	{
		return buffer;
	}

}

