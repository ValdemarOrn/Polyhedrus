#ifndef LEIFTUR_CV_FREQ
#define LEIFTUR_CV_FREQ

#include "AudioLib/Utils.h"

namespace Leiftur
{
	class CvFreq
	{
	private:
		static double GetCvFreqHz(double cv);
		static double Warp(double freqHz, double samplerate);

		float CvToFreq[11000];
		float CvToFreqWarped[11000];

	public:
		void Initialize(float samplerate);
		float inline GetFreq(float cv)
		{
			cv = AudioLib::Utils::Limit(cv, 0.0f, 11.0f);
			return CvToFreq[(int)(cv * 999.99)];
		}

		float inline GetFreqWarped(float cv)
		{
			cv = AudioLib::Utils::Limit(cv, 0.0f, 11.0f);
			return CvToFreqWarped[(int)(cv * 999.99)];
		}

	};
}

#endif