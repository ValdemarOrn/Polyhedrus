#ifndef LEIFTUR_FILTER_CASCADE_ZERO
#define LEIFTUR_FILTER_CASCADE_ZERO

#include <cmath>

#include "CvFreq.h"

namespace Leiftur
{
	struct ZeroDelay2Lp
	{
		float z1State;
		float y;
		double g2;
		//float g;

		inline float Process(float x)
		{
			// perform one sample tick of the lowpass filter
			//float v = (x - z1State) * g / (1 + g);
			
			double v = (x - z1State) * g2;
			y = v + z1State;
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
	public:
		static inline float GetCvFreq(float cv)
		{
			// Voltage is 1V/OCt, C0 = 16.3516Hz
			// 10.3V = Max = 20614.33hz
			float freq = (float)(440.0 * std::pow(2, (cv * 12 - 69.0 + 12) / 12));
			return freq;
		}

	public:
		float Drive;
		float Cutoff;
		float Resonance;
		float CutoffMod;
		float ResonanceMod;
		float DriveMod;
				
	private:
		float* buffer;
		float gain;
		float gainInv;
		float totalResonance;

		float k;
		float uScaler;
		float gainCompensation;
		float g, g2, g3, g4;
		CvFreq cvToFreq;
		ZeroDelay2Lp lp1;
		ZeroDelay2Lp lp2;
		ZeroDelay2Lp lp3;
		ZeroDelay2Lp lp4;

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
		inline float* GetOutput() { return buffer; }
		
	private:
		void ProcessSample(float input);
		void Update();
	};
}

#endif

