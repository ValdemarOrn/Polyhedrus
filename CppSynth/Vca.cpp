#include "Vca.h"

namespace Leiftur
{
	Vca::Vca()
	{
		ControlVoltage = 0.0;
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
		for (size_t i = 0; i < len; i++)
		{
			buffer[i] = input[i] * ControlVoltage;
		}
	}

	float* Vca::GetOutput()
	{
		return buffer;
	}

}

