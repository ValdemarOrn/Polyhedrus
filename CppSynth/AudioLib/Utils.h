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

		static inline float TanhPoly(float x)
		{
			float sign = (x >= 0) ? 1.0 : -1.0;

			x = x * sign;
			float xSquare = x * x;
			float xCube = xSquare * x;
			float result = 1.0f - 1.0 / (1 + x + xSquare + xCube);
			return result * sign;
		}
	}
}

#endif
