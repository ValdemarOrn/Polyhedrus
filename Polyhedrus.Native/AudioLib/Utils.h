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

		static inline float fastabs(float f)
		{
			int i = ((*(int*)&f) & 0x7fffffff); return (*(float*)&i);
		}

		static inline float fastneg(float f)
		{
			int i = ((*(int*)&f) ^ 0x80000000); return (*(float*)&i);
		}

		static inline int fastsgn(float f)
		{
			return 1 + (((*(int*)&f) >> 31) << 1);
		}

		static inline float Note2FreqT(float note, float A4Tuning = 440.0f)
		{
			return (float)(A4Tuning * std::powf(2.0f, (note - 69.0f) / 12.0f));
		}

		static inline float Freq2NoteT(float hz, float A4Tuning = 440.0f)
		{
			if (hz <= 0.0001f)
				return -195.8288651168893f;

			return std::log2f(hz / A4Tuning) * 12.0f + 69.0f;
		}

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
				buffer[i] = 0.0f;
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

		static inline float LinInterp(float* table, int tableSize, float value)
		{
			if (value <= 0)
				return table[0];
			if (value >= 1.0)
				return table[tableSize - 1];

			float fIdx = value * tableSize;
			int idxA = (int)fIdx;
			int idxB = idxA + 1;
			if (idxA >= tableSize) idxA = tableSize - 1;
			if (idxB >= tableSize) idxB = tableSize - 1;
			float mix = fIdx - idxA;

			return table[idxA] * (1 - mix) + table[idxB] * mix;
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
			int sign = fastsgn(x);

			x = fastabs(x);
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

		static inline float CubicNonlin(float x)
		{
			// Eq1: x - a * x^3
			// derivative is 
			// Eq2: 1 - 3a * x^2
			// saturation at +-1.5
			// solving for Eq1 = 1, Eq2 = 0, we get
			// = 0.5 / (3*(1.5)^2-(1.5)^3)
			// == 0.148148148148148

			const float a = 0.148148148148148f;
			int s1 = x > -1.5f;
			int s2 = x > 1.5f;

			float y = x - a * x*x*x;
			float vals[3] = { -1.0f, y, 1.0f };
			return vals[s1 + s2];
		};

		static inline void GainAndSum(float* source, float* dest, float gain, int len)
		{
			for (int i = 0; i < len; i++)
				dest[i] += source[i] * gain;
		}

		static inline double DB2gain(double input)
		{
			return std::pow(10, input / 20);
		}

		static inline float Gain2DB(float input)
		{
			return 20.0f * std::log10(input);
		}

		static inline double Rms(float* data, int len)
		{
			double sum = 0.0;

			for (int i = 0; i < len; i++)
			{
				sum += data[i] * data[i];
			}

			return std::sqrt(sum / len);
		}
	};
}

#endif
