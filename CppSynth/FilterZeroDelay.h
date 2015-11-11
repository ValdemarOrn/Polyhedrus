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
		//float g;
		float g2;

		inline float Process(float x)
		{
			// perform one sample tick of the lowpass filter
			//float v = (x - z1_state) * g / (1 + g);
			float v = (x - z1_state) * g2;
			float y = v + z1_state;
			z1_state = y + v;
			return y;
		}

		inline void SetG(float g)
		{
			g2 = g / (1 + g);
		}
	};

	class FilterZeroDelay
	{
	public:
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
		int oversample;
		float oversampleInv;

		int samplerate;
		float T;
		int updateCounter;
		int modulationUpdateRate;

	public:
		FilterZeroDelay();
		~FilterZeroDelay();

		void Initialize(int samplerate, int oversampling, int bufferSize, int modulationUpdateRate);
		void Process(float* input, int len);
		float* GetOutput();

	private:
		float ProcessSample(float input);
		void Update();
	};
}

#endif

