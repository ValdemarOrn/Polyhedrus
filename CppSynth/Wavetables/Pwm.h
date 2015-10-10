
class Pwm
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
				wave[i] = i / (double)sampleCount < dutyCycle ? -1 : 1;
			}
		}

		return table;
	}
};
