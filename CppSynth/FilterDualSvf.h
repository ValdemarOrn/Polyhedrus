#ifndef LEIFTUR_FILTER_DUAL_SVF
#define LEIFTUR_FILTER_DUAL_SVF

#include "AudioLib/SvfFilter.h"
#include "CvFreq.h"

namespace Leiftur
{
	class FilterDualSvf
	{
	public:
		const int Oversample = 4;

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
		CvFreq cvToFreq;
		AudioLib::SvfFilter svf1;
		AudioLib::SvfFilter svf2;

		float fsinv;
		int samplerate;
		int updateCounter;
		int modulationUpdateRate;

	public:
		FilterDualSvf();
		~FilterDualSvf();

		void Initialize(int samplerate, int bufferSize, int modulationUpdateRate);
		void Process(float* input, int len);
		inline float* GetOutput() { return buffer; }

	private:
		void Update();
	};

}

#endif
