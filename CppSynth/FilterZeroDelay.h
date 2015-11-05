#ifndef LEIFTUR_FILTER_ZERO_DELAY
#define LEIFTUR_FILTER_ZERO_DELAY

#define _USE_MATH_DEFINES
#include <cmath>

#include "CvFreq.h"


namespace Leiftur
{
	struct ZeroDelayLp
	{
		float z1_state;
		float g;

		inline float Process(float x)
		{
			// perform one sample tick of the lowpass filter
			float v = (x - z1_state) * g / (1 + g);
			float y = v + z1_state;
			z1_state = y + v;
			return y;
		}
	};

	class FilterZeroDelay
	{
	public:
		const int Oversample = 4;

		float Drive;
		float Cutoff;
		float Resonance;
		float CutoffMod;
		float ResonanceMod;
		float DriveMod;

		CvFreq cvToFreq;
		ZeroDelayLp lp1;
		ZeroDelayLp lp2;
		ZeroDelayLp lp3;
		ZeroDelayLp lp4;

	private:
		float* buffer;
		float gain;
		float driveTotal;
		float totalResonance;
		float feedback;

		int samplerate;
		float T;
		int updateCounter;
		int modulationUpdateRate;

	public:
		FilterZeroDelay();
		~FilterZeroDelay();

		void Initialize(int samplerate, int bufferSize, int modulationUpdateRate);
		void Process(float* input, int len);
		float* GetOutput();

	private:
		float ProcessSample(float input);
		void Update();
	};
}

#endif

