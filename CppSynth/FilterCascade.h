#ifndef LEIFTUR_FILTER_CASCADE
#define LEIFTUR_FILTER_CASCADE

#include <cmath>

namespace Leiftur
{
	class FilterCascade
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
		const int Oversample = 2;

		float Drive;
		float Cutoff;
		float Resonance;
		float CutoffMod;
		float ResonanceMod;
		float DriveMod;

		float VA;
		float VB;
		float VC;
		float VD;
		float VX;

	private:
		float* buffer;
		float gain;
		float driveTotal;
		float totalResonance;
		float oversampledInput;

		float p;
		float x;
		float a;
		float b;
		float c;
		float d;
		float feedback;

		float fsinv;
		int samplerate;
		int updateCounter;
		int modulationUpdateRate;

	public:
		FilterCascade();
		~FilterCascade();

		void Initialize(int samplerate, int bufferSize, int modulationUpdateRate);
		void Process(float* input, int len);
		float* GetOutput();

	private:
		float ProcessSample(float input);
		void Update();
	};
}

#endif

