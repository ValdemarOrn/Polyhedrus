#include "Utils.h"
#include <cmath>

float note2Freq[129];

void AudioLib::Utils::Initialize()
{
	for (int i = 0; i < 12800; i++)
	{
		note2Freq[i] = 440.0 * std::pow(2, (0.01*i - 69) / 12.0);
	}
}

float AudioLib::Utils::Note2Freq(float note)
{
	auto base = (int)(note * 100);
	auto partial = note * 100 - base;

	auto freq = note2Freq[base];
	auto slope = note2Freq[base + 1] - freq;
	
	return freq + partial * slope;
}
