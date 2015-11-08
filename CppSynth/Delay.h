#ifndef LEIFTUR_DELAY
#define LEIFTUR_DELAY

#include "Parameters.h"

namespace Leiftur
{
	class Delay
	{
	public:
		float Crossfeed;
		float DelayL;
		float DelayR;
		float Diffuse;
		float FeedbackL;
		float FeedbackR;
		float Highpass;
		float Lowpass;
		float Saturate;
		bool Sync;
		float Wet;
		bool IsEnabled;

		float DelayLMod;
		float DelayRMod;
		float FeedbackLMod;
		float FeedbackRMod;
		float HighpassMod;
		float LowpassMod;
		float SaturateMod;

	private:
		int delayBufferSize;
		float* bufferL;
		float* bufferR;
		float* delayLineL;
		float* delayLineR;
		int samplerate;
		int modulationUpdateRate;
		int updateCounter;
		int samplePos;

		int delaySamplesL;
		int delaySamplesR;
		float totalFeedbackL;
		float totalFeedbackR;
		float totalSaturate;

	public:
		Delay();
		~Delay();
		void Initialize(int samplerate, int bufferSize, int modulationUpdateRate);
		void SetParameter(DelayParameters parameter, double value);
		void Process(float* inputL, float* inputR, int len);
		float* GetOutputL();
		float* GetOutputR();
	private:
		void Update();
		int GetIndex(int offset);
	};

}

#endif

