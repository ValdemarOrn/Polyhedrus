#ifndef AUDIOLIB_SSE
#define AUDIOLIB_SSE

#include <intrin.h>

namespace AudioLib
{
	class Sse
	{
	public:

		static inline void Min(float* input, float* output, const float min, int len)
		{
			__m128 mini = _mm_load1_ps(&min);
			__m128* inputPtr = (__m128*)input;
			__m128* outputPtr = (__m128*)output;
			for (int i = 0; i < len / 4; i ++)
			{
				outputPtr[i] = _mm_min_ps(inputPtr[i], mini);
			}
		}

		static inline void Max(float* input, float* output, const float max, int len)
		{
			__m128 maxi = _mm_load1_ps(&max);
			__m128* inputPtr = (__m128*)input;
			__m128* outputPtr = (__m128*)output;
			for (int i = 0; i < len / 4; i++)
			{
				outputPtr[i] = _mm_max_ps(inputPtr[i], maxi);
			}
		}

		static inline void Floor(float* input, int len)
		{
			__m128* inputPtr = (__m128*)input;
			for (int i = 0; i < len / 4; i++)
			{
				inputPtr[i] = _mm_floor_ps(inputPtr[i]);
			}
		}

		static inline void ConvertToFloats(int* input, float* output, int len)
		{
			__m128i* inputPtr = (__m128i*)input;
			__m128* outputPtr = (__m128*)output;

			for (int i = 0; i < len / 4; i++)
			{
				outputPtr[i] = _mm_cvtepi32_ps(inputPtr[i]);
			}
		}

	};

}

#endif
