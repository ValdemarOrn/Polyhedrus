#ifndef AUDIOLIB_UTILS
#define AUDIOLIB_UTILS

#include <cstring>

namespace AudioLib
{
	namespace Utils
	{
		void Initialize();
		float Note2Freq(float note);

		static inline void ZeroBuffer(float* buffer, int len)
		{
			for (int i = 0; i < len; i++)
				buffer[i] = 0.0;
		}

		static inline void Copy(float* source, float* dest, int len)
		{
			std::memcpy(dest, source, len * sizeof(float));
		}

		static inline void Gain(float* buffer, float gain, int len)
		{
			for (int i = 0; i < len; i++)
			{
				buffer[i] *= gain;
			}
		}

		static inline float Limit(float input, float min, float max)
		{
			int above = (input > min);
			int notAbove = above != 1;
			input = above * input + notAbove * min;

			int below = (input < max);
			int notBelow = below != 1;
			input = below * input + notBelow * max;

			return input;
		}

		static inline float LimitMin(float input, float min)
		{
			int above = (input > min);
			int notAbove = above != 1;
			return above * input + notAbove * min;
		}

		static inline float LimitMax(float input, float max)
		{
			int below = (input < max);
			int notBelow = below != 1;
			return below * input + notBelow * max;
		}

		static inline float TanhPoly(float x)
		{
			float sign = (x >= 0) ? 1.0f : -1.0f;

			x = x * sign;
			float xSquare = x * x;
			float xCube = xSquare * x;
			float result = 1.0f - 1.0f / (1.0f + x + xSquare + xCube);
			return result * sign;
		}

		static inline void GainAndSum(float* dest, float* source, float gain, int len)
		{
			for (int i = 0; i < len; i++)
				dest[i] += source[i] * gain;
		}
	}
}

#endif
