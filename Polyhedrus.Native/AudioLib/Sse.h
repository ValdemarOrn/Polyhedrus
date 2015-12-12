#ifndef AUDIOLIB_SSE
#define AUDIOLIB_SSE

#include <intrin.h>

namespace AudioLib
{
	class Sse
	{
	public:

		static void Min(float* const input, const float min, const int len)
		{
			__m128 mini = _mm_load1_ps(&min);
			__m128* inputPtr = (__m128*)input;
			for (int i = 0; i < (len / 4); i++)
			{
				inputPtr[i] = _mm_min_ps(inputPtr[i], mini);
			}
		}

		static void Max(float* const input, const float max, const int len)
		{
			__m128 maxi = _mm_load1_ps(&max);
			__m128* inputPtr = (__m128*)input;
			for (int i = 0; i < (len / 4); i++)
			{
				inputPtr[i] = _mm_max_ps(inputPtr[i], maxi);
			}
		}

		static inline void Floor(float* const input, const int len)
		{
			__m128* inputPtr = (__m128*)input;
			for (int i = 0; i < len / 4; i++)
			{
				inputPtr[i] = _mm_floor_ps(inputPtr[i]);
			}
		}

		static inline void ConvertToFloats(const int* const input, float* const output, const int len)
		{
			__m128i* inputPtr = (__m128i*)input;
			__m128* outputPtr = (__m128*)output;

			for (int i = 0; i < len / 4; i++)
			{
				outputPtr[i] = _mm_cvtepi32_ps(inputPtr[i]);
			}
		}

		// Returns a 16-byte aligned array of type T
		template<typename T>
		static inline T* AlignedMalloc(int size)
		{
			T* result = (T*)_aligned_malloc(size * sizeof(T), 16);
			return result;
		}

		template<typename T>
		static inline void AlignedFree(T* ptr)
		{
			_aligned_free(ptr);
		}

	};

}

#endif
