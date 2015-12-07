#ifndef POLYHEDRUS_DELAY
#define POLYHEDRUS_DELAY

#include "AudioLib/OnePoleFilters.h"
#include "Parameters.h"
#include "AllpassDiffuser.h"
#include "AudioLib/TempoSync.h"
#include "ParameterFormatters.h"

namespace Polyhedrus
{
	class Delay
	{
	public:
		bool IsEnabled;
		double Bpm;

	private:
		float diffuseAmount;
		float delayL;
		float delayR;
		float feedbackL;
		float feedbackR;
		float highpass;
		float lowpass;
		float saturate;
		bool sync;
		float wet;

		AudioLib::Lp1 lpL;
		AudioLib::Lp1 lpR;
		AudioLib::Hp1 hpL;
		AudioLib::Hp1 hpR;

		AllpassDiffuser diffuserL;
		AllpassDiffuser diffuserR;

		int delayBufferSize;
		float* bufferL;
		float* bufferR;
		float* delayLineL;
		float* delayLineR;
		int samplerate;
		float T;
		int samplePos;

		int delaySamplesL;
		int delaySamplesR;
		float totalFeedbackL;
		float totalFeedbackR;
		float totalSaturate;
		float satInner;
		float satOuter;
		float dryGain;
		float wetGain;

		AudioLib::Quantization delayQL;
		AudioLib::Quantization delayQR;
		double delayTimeL;
		double delayTimeR;

	public:
		Delay();
		~Delay();
		void Initialize(int samplerate, int bufferSize);
		void SetParameter(DelayParameters parameter, double value);
		void Process(float* inputL, float* inputR, int len);
		float* GetOutputL();
		float* GetOutputR();
		void ClearBuffers();
		inline std::string GetDelayLString()
		{
			return sync ? AudioLib::TempoSync::ToString(delayQL) : (ParameterFormatters::FormatIntRounded(delayTimeL * 1000) + "ms");
		}
		inline std::string GetDelayRString()
		{
			return sync ? AudioLib::TempoSync::ToString(delayQR) :( ParameterFormatters::FormatIntRounded(delayTimeR * 1000) + "ms");
		}
	private:
		void Update();
		int GetIndex(int offset);
	};

}

#endif

