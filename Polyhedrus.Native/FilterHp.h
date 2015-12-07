#ifndef POLYHEDRUS_FILTER_HP
#define POLYHEDRUS_FILTER_HP

#include "AudioLib/SvfFilter.h"
#include "Parameters.h"
#include "CvFreq.h"

namespace Polyhedrus
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
		AudioLib::SvfFilter svf;
		CvFreq cvToFreq;

	public:
		FilterHp();
		~FilterHp();
		void Initialize(int samplerate, int bufferSize, int modulationUpdateRate);
		void SetParameter(FilterHpParameters parameter, double value);
		void Process(float* input, int len);
		inline float* GetOutput() { return buffer; }
		inline float GetCutoff() { return svf.Fc; }
		std::vector<uint8_t> GetVisual(FilterHpParameters parameter, int* baseLevel);

		static std::vector<uint8_t> GetKeytrackVisual(double keytrack);
	private:
		void Update();
	};
}

#endif
