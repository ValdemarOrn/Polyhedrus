using System;

namespace CreateWavetables
{
	internal class Pwm
	{
		internal static float[][] CreateTable(int sampleCount, int numTables)
		{
			var table = new float[numTables][];
			for (int t = 0; t < table.Length; t++)
			{
				var wave = new float[sampleCount];
				table[t] = wave;

				var dutyCycle = 0.5 + 0.5 * t / (double)numTables;

				for (int i = 0; i < sampleCount; i++)
				{
					wave[i] = i / (double)sampleCount < dutyCycle ? -1 : 1;
				}
			}

			return table;
		}
	}
}