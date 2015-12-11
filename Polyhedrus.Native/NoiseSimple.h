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
		float Type;
		//float TypeMod;
	private:
		AudioLib::LcgRandom random;
		AudioLib::Lp1 lpL;
		AudioLib::Lp1 lpR;
		AudioLib::Hp1 hpL;
		AudioLib::Hp1 hpR;
		float* bufferL;
		float* bufferR;
		float* randBuf;
		float* output[2];
		
		int samplerate;
		int bufferSize;
		float brownL;
		float brownR;
		float typeTotal;
				
	public:
		NoiseSimple();
		~NoiseSimple();
		void Initialize(int samplerate, int bufferSize, int voiceIndex);
		void Process(int len);
		inline float** GetOutput() { return output; }

	};
}

#endif