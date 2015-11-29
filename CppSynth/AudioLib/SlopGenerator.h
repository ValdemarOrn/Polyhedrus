#ifndef AUDIOLIB_SLOP_GENERATOR
#define AUDIOLIB_SLOP_GENERATOR

#include <cmath>
#include "MathDefs.h"
#include "LcgRandom.h"

namespace AudioLib
{
	class SlopGenerator
	{
	public:
		float Output;
		float Fc;

		inline SlopGenerator()
		{
			Output = 0.0f;
			Fc = 0.1f;

			fcSqrtInv = 0.1;
			alpha = 0.1;
			value = 0.0;
			zState1 = 0.0;
			zState2 = 0.0;
			samplerate = 48000;
			fsInv = 1.0 / 48000.0;
			updateShRate = 10;
			updateCounter = 0;
		}

		inline void Initialize(int samplerate, int modulationUpdateRate, int seed)
		{
			this->samplerate = samplerate / modulationUpdateRate;
			fsInv = 1.0f / this->samplerate;
			rand.SetSeed(seed * 12345);

			updateShRate = this->samplerate / 1000;
			updateCounter = 0;
		}

		inline void Update()
		{
			alpha = std::sin(M_PI * Fc * fsInv);
			fcSqrtInv = 1.0 / std::sqrt(Fc);
		}

		inline float Process()
		{
			if (updateCounter == 0)
			{
				Update();
				value = (2 * rand.NextDouble() - 1);
				updateCounter = updateShRate;
			}

			zState1 = zState1 * (1 - alpha) + value * alpha;
			zState2 = zState2 * (1 - alpha) + zState1 * alpha;

			Output = zState2 * fcSqrtInv * 20; // normalized to 1Hz
			updateCounter--;
			return Output;
		}

	private:
		float fcSqrtInv;
		float alpha;
		float value;
		float zState1;
		float zState2;
		int samplerate;
		float fsInv;
		int updateShRate;
		int updateCounter;
		LcgRandom rand;
	};
}

#endif
