#define _USE_MATH_DEFINES
#include <cmath>

#include "Utils.h"

float AudioLib::Utils::note2Freq[12800];
float AudioLib::Utils::sintable[TableSize];
float AudioLib::Utils::costable[TableSize];
float AudioLib::Utils::tableScaler;

void AudioLib::Utils::Initialize()
{
	for (int i = 0; i < 12800; i++)
	{
		note2Freq[i] = 440.0 * std::pow(2, (0.01*i - 69) / 12.0);
	}

	tableScaler = 1.0 / (2 * M_PI) * TableSize;

	for (int i = 0; i < 20000; i++)
	{
		sintable[i] = std::sin(i / (double)TableSize * M_PI);
		costable[i] = std::cos(i / (double)TableSize * M_PI);
	}
}

float AudioLib::Utils::Note2Freq(float note)
{
	auto base = (int)(note * 100);
	auto partial = note * 100 - base;

	if (base > 12799)
	{
		base = 12799;
		partial = 0;
	}

	auto freq = note2Freq[base];
	auto slope = note2Freq[base + 1] - freq;
	
	return freq + partial * slope;
}
