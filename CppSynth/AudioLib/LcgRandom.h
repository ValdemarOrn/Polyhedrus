#ifndef AUDIOLIB_LCGRANDOM
#define AUDIOLIB_LCGRANDOM

#include <stdint.h>

namespace AudioLib
{
	class LcgRandom
	{
	private:
		uint64_t x;
		uint64_t a;
		uint64_t c;
		//uint64_t m;

		double doubleInv;
		float floatInv;

	public:
		inline LcgRandom(uint64_t seed = 0)
		{
			x = seed;
			//m = 1 << 32;
			a = 22695477;
			c = 1;

			doubleInv = 1.0 / (double)UINT32_MAX;
			floatInv = 1.0 / (float)UINT32_MAX;
		}

		inline uint32_t NextInt()
		{
			uint64_t axc = a * x + c;
			//x = axc % m;
			x = axc & 0xFFFFFFFF;
			return x;
		}

		inline double NextDouble()
		{
			auto n = NextInt();
			return n * doubleInv;
		}

		inline double NextFloat()
		{
			auto n = NextInt();
			return n * floatInv;
		}
	};
}

#endif