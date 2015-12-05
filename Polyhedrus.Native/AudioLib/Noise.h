#ifndef AUDIOLIB_NOISE
#define AUDIOLIB_NOISE

#include <stdint.h>

namespace AudioLib
{
	class Noise
	{
	public:
		static const int NoiseSamples = 96000;

	private:
		static uint8_t noise[NoiseSamples];

	public:
		static void Initialize();

		static inline uint8_t Random(int index)
		{
			return noise[index];
		}
	};
}

#endif