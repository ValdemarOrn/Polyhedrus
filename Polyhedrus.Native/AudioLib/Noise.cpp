
#include "Noise.h"
#include <cmath>

namespace AudioLib
{
	uint8_t Noise::noise[96000];
	
	void Noise::Initialize()
	{
		for (size_t i = 0; i < NoiseSamples; i++)
			noise[i] = std::rand() & 0xFF;
	}

}