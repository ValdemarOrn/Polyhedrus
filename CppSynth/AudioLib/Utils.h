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
	}
}

#endif
