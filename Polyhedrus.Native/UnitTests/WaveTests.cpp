#include "WaveTests.h"
#include "../AudioLib/WaveFile.h"
#include <cassert>
#include <cmath>

namespace Tests
{
	namespace Wave
	{
		void TestWaveReader1()
		{
			std::string filename("C:\\Src\\_Tree\\Audio\\Polyhedrus\\Wavetables\\PWM.wav");
			auto data = AudioLib::WaveFile::ReadWaveFile(filename);

			assert(data.size() == 1);
			auto wave = data.at(0);

			assert(wave.size() == 256 * 2048);
			assert(std::abs(wave.at(0) - 0.8) < 0.01);
			assert(std::abs(wave.at(7) - 0.8) < 0.01);
			assert(std::abs(wave.at(8) + 0.8) < 0.01);

		}
	}
}