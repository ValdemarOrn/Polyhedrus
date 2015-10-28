#ifndef LEIFTUR_FILTER_ZERO_DELAY
#define LEIFTUR_FILTER_ZERO_DELAY

#include <cmath>

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
		static inline float GetCvFreq(float cv)
		{
			// Voltage is 1V/OCt, 0V = C0 = 16.3516Hz
			// 10.3V = Max = 20614.33hz
			float freq = (float)(440.0 * std::pow(2, (cv * 12 - 69.0 + 12) / 12));
			return freq;
		}

	public:
		const int Oversample = 4;

		float Drive;
		float Cutoff;
		float Resonance;
		float CutoffMod;
		float ResonanceMod;
		float DriveMod;

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

