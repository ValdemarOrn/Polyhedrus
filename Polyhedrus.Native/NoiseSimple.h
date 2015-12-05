#ifndef POLYHEDRUS_NOISE_SIMPLE
#define POLYHEDRUS_NOISE_SIMPLE

#include "AudioLib/OnePoleFilters.h"
#include "AudioLib/LcgRandom.h"

namespace Polyhedrus
{
	enum class NoiseType
	{
		White = 0,
		Pink = 1,
		Brown = 2
	};

	class NoiseSimple
	{
	public:
		NoiseType Type;
	private:
		AudioLib::LcgRandom random;
		AudioLib::Lp1 lpL;
		AudioLib::Lp1 lpR;
		float* bufferL;
		float* bufferR;
		float* output[2];
		int samplerate;
		int bufferSize;
		float L;
		float R;
		float hpRolloff;
		
	public:
		NoiseSimple();
		~NoiseSimple();
		void Initialize(int samplerate, int bufferSize);
		void Process(int len);
		inline float** GetOutput() { return output; }

	};
}

#endif