#ifndef LEIFTUR_FILTER_HP
#define LEIFTUR_FILTER_HP

#include "AudioLib/Biquad.h"
#include "Parameters.h"
#include "CvFreq.h"

namespace Leiftur
{
	class FilterHp
	{
	public:
		bool IsEnabled;
		float Cutoff;
		float Resonance;
		float CutoffMod;
		float ResonanceMod;

	private:
		float* buffer;
		int samplerate;
		int modulationUpdateRate;
		int updateCounter;
		AudioLib::Biquad biquad;
		CvFreq cvToFreq;

	public:
		FilterHp();
		~FilterHp();
		void Initialize(int samplerate, int bufferSize, int modulationUpdateRate);
		void SetParameter(FilterHpParameters parameter, double value);
		void Process(float* input, int len);
		float* GetOutput();
	private:
		void Update();
	};
}

#endif
