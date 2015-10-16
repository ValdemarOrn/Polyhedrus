#ifndef LEIFTUR_WAVETABLES_SAWTOOTH
#define LEIFTUR_WAVETABLES_SAWTOOTH

#include <cmath>

namespace Leiftur
{
	namespace Wavetables
	{
		class Sawtooth
		{
		public:
			static inline float* CreateTable(int sampleCount, int numTables)
			{
				auto table = new float[numTables * sampleCount];

				for (int t = 0; t < numTables; t++)
				{
					auto wave = &table[t * sampleCount];
					auto dutyCycle = 0.5 + 0.5 * t / (double)numTables;

					for (int i = 0; i < sampleCount; i++)
					{
						auto phaseA = i / (double)sampleCount;
						auto phaseB = i / (double)sampleCount + t / (double)numTables;
						if (phaseB >= 1.0) 
							phaseB -= 1.0;

						auto wa = 1.0 - 2.0 * phaseA;
						auto wb = 1.0 - 2.0 * phaseB;

						wave[i] = wa + wb;
					}
				}

				return table;
			}
		};
	}
}

#endif
