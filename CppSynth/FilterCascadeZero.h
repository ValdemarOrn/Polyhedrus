#ifndef LEIFTUR_FILTER_CASCADE_ZERO
#define LEIFTUR_FILTER_CASCADE_ZERO

#include <cmath>

#include "CvFreq.h"

namespace Leiftur
{
	struct ZeroDelay2Lp
	{
		float z1State;
		//float g;
		double g2;

		inline float Process(float x)
		{
			// perform one sample tick of the lowpass filter
			//float v = (x - z1State) * g / (1 + g);
			double v = (x - z1State) * g2;
			float y = v + z1State;
			z1State = y + v;
			return y;
		}

		inline void SetG(float g)
		{
			//this->g = g;
			g2 = g / (1 + g);
		}
	};

	class FilterCascadeZero
	{
	private:
		static const int CVtoAlphaSize = 10500;
		static float CVtoAlpha[CVtoAlphaSize];
		static void ComputeCVtoAlpha(int samplerate);
	public:
		static inline float GetCvFreq(float cv)
		{
			// Voltage is 1V/OCt, C0 = 16.3516Hz
			// 10.3V = Max = 20614.33hz
			float freq = (float)(440.0 * std::pow(2, (cv * 12 - 69.0 + 12) / 12));
			return freq;
		}

	public:
		const int Oversample = 1;

		float Drive;
		float Cutoff;
		float Resonance;
		float CutoffMod;
		float ResonanceMod;
		float DriveMod;

		CvFreq cvToFreq;
		ZeroDelay2Lp lp1;
		ZeroDelay2Lp lp2;
		ZeroDelay2Lp lp3;
		ZeroDelay2Lp lp4;
		
	private:
		float* buffer;
		float gain;
		float gainInv;
		float driveTotal;
		float totalResonance;

		float k;
		float uScaler;
		float gainCompensation;
		float g;
		float g2;
		float g3;
		float g4;

		float fsinv;
		int samplerate;
		int bufferSize;
		int updateCounter;
		int modulationUpdateRate;

	public:
		FilterCascadeZero();
		~FilterCascadeZero();

		void Initialize(int samplerate, int bufferSize, int modulationUpdateRate);
		void Process(float* input, int len);
		float* GetOutput();

	private:
		float ProcessSample(float input);
		void Update();
	};
}

#endif

