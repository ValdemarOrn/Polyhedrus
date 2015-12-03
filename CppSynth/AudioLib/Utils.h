#ifndef AUDIOLIB_UTILS
#define AUDIOLIB_UTILS

#include <cstring>
#include <cmath>

namespace AudioLib
{
	class Utils
	{
	private:
		static const int TableSize = 20000;
		static float note2Freq[12800];
		static float sintable[TableSize];
		static float costable[TableSize];
		static float tableScaler;

		static const int TanhTableSize = 60000;
		static float tanhTable[TanhTableSize];

	public:
		static void Initialize();
		static float Note2Freq(float note);

		static inline float FastSin(float x)
		{
			int idx = ((int)(x * tableScaler) + 100 * TableSize) % TableSize;
			return sintable[idx];
		}

		static inline float FastCos(float x)
		{
			int idx = ((int)(x * tableScaler) + 100 * TableSize) % TableSize;
			return costable[idx];
		}

		static inline void ZeroBuffer(float* buffer, int len)
		{
			for (int i = 0; i < len; i++)
				buffer[i] = 0.0;
		}
		
		static inline void PreventDenormal(float* buffer, int len)
		{
			static bool sign;
			auto offset = 0.00001f;
			if (sign) offset *= -1;
			sign = !sign;

			for (int i = 0; i < len; i++)
			{
				buffer[i] = offset;
				offset *= -1;
			}
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
			float sign = (float)(-1.0 + 2 * (x >= 0));

			x = x * sign;
			float xSquare = x * x;
			float xCube = xSquare * x;
			float result = 1.0f - 1.0f / (1.0f + x + xSquare + xCube);
			return result * sign;
		}

		static inline float TanhLookup(float x)
		{
			int i = (int)(x * 10000 + 30000);
			int overZero = i > 0;
			int underMax = i < TanhTableSize - 1;
			i = i * overZero;
			i = i * underMax + !underMax * (TanhTableSize - 1);
			return tanhTable[i];
		}

		static inline float QuickNonlin(float x)
		{
			float sign = (float)(-1.0 + 2 * (x >= 0));

			x = x * sign;
			if (x > 2)
				return sign;
			
			float part = 1.0f - x * 0.5f;
			return (1 - part * part) * sign;
		}

		static inline float Cubic6Nonlin(float x)
		{
			return (std::abs(x) > 1.4142135623) 
				? (x < 0.0 ? -0.9428090416f : 0.9428090416f)
				: (x - (x*x*x) / 6);
		};

		static inline void GainAndSum(float* source, float* dest, float gain, int len)
		{
			for (int i = 0; i < len; i++)
				dest[i] += source[i] * gain;
		}
	};
}

#endif
