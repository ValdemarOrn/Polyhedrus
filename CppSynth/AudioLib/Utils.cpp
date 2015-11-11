#define _USE_MATH_DEFINES
#include <cmath>

#include "Utils.h"

namespace AudioLib
{
	float Utils::note2Freq[12800];
	float Utils::sintable[TableSize];
	float Utils::costable[TableSize];
	float Utils::tableScaler;

	float Utils::tanhTable[TanhTableSize];

	void Utils::Initialize()
	{
		for (int i = 0; i < 12800; i++)
		{
			note2Freq[i] = (float)(440.0 * std::pow(2, (0.01*i - 69) / 12.0));
		}

		tableScaler = (float)(1.0 / (2.0f * M_PI) * TableSize);

		for (int i = 0; i < TableSize; i++)
		{
			sintable[i] = (float)std::sin(i / (double)TableSize * M_PI);
			costable[i] = (float)std::cos(i / (double)TableSize * M_PI);
		}

		for (int i = 0; i < TanhTableSize; i++)
		{
			tanhTable[i] = std::tanh(-3.0 + i / 10000.0);
		}
	}

	float Utils::Note2Freq(float note)
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
}
