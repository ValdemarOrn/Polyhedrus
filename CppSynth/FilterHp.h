#ifndef LEIFTUR_FILTER_HI
#define LEIFTUR_FILTER_HI

#include "AudioLib/Biquad.h"
#include "Parameters.h"

namespace Leiftur
{
	class FilterHp
	{
	public:
		float Cutoff;
		float Resonance;
		float CutoffMod;

	private:
		float* buffer;
		int samplerate;
		int modulationUpdateRate;
		int updateCounter;
		AudioLib::Biquad biquad;

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
