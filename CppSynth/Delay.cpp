#include "Delay.h"
#include "AudioLib/Utils.h"
#include "AudioLib/ValueTables.h"

using namespace AudioLib;

namespace Leiftur
{
	Delay::Delay()
	{
		Crossfeed = 0;
		DelayL = 0;
		DelayR = 0;
		Diffuse = 0;
		FeedbackL = 0;
		FeedbackR = 0;
		Highpass = 0;
		Lowpass = 0;
		Saturate = 0;
		Sync = false;
		Wet = 0;
		IsEnabled = true;

		DelayLMod = 0;
		DelayRMod = 0;
		FeedbackLMod = 0;
		FeedbackRMod = 0;
		HighpassMod = 0;
		LowpassMod = 0;
		SaturateMod = 0;

		bufferL = 0;
		bufferR = 0;
		delayLineL = 0;
		delayLineR = 0;
		samplerate = 48000;
		modulationUpdateRate = 8;
		updateCounter = 0;
		samplePos = 0;

		delaySamplesL = 0;
		delaySamplesR = 0;
		totalFeedbackL = 0;
		totalFeedbackR = 0;
		totalSaturate = 0;
		delayBufferSize = 0;
	}

	Delay::~Delay()
	{
		delete bufferL;
		delete bufferR;
		delete delayLineL;
		delete delayLineR;
	}

	void Delay::Initialize(int samplerate, int bufferSize, int modulationUpdateRate)
	{
		delayBufferSize = samplerate * 2;
		bufferL = new float[bufferSize];
		bufferR = new float[bufferSize];
		delayLineL = new float[delayBufferSize];
		delayLineR = new float[delayBufferSize];
		this->modulationUpdateRate = modulationUpdateRate;
		this->samplerate = samplerate;
	}

	void Delay::SetParameter(DelayParameters parameter, double value)
	{
		switch (parameter)
		{
		case DelayParameters::Crossfeed:
			Crossfeed = (float)value;
			break;
		case DelayParameters::DelayL:
			DelayL = (float)value;
			break;
		case DelayParameters::DelayR:
			DelayR = (float)value;
			break;
		case DelayParameters::Diffuse:
			Diffuse = (float)value;
			break;
		case DelayParameters::FeedbackL:
			FeedbackL = (float)value;
			break;
		case DelayParameters::FeedbackR:
			FeedbackR = (float)value;
			break;
		case DelayParameters::Highpass:
			Highpass = (float)value;
			break;
		case DelayParameters::Lowpass:
			Lowpass = (float)value;
			break;
		case DelayParameters::Saturate:
			Saturate = (float)value;
			break;
		case DelayParameters::Sync:
			Sync = value >= 0.5;
			break;
		case DelayParameters::Wet:
			Wet = (float)value;
			break;
		}
	}

	void Delay::Process(float* inputL, float* inputR, int len)
	{
		if (!IsEnabled)
		{
			AudioLib::Utils::Copy(inputL, bufferL, len);
			AudioLib::Utils::Copy(inputR, bufferR, len);
			return;
		}

		for (int i = 0; i < len; i++)
		{
			if (updateCounter <= 0)
			{
				Update();
				updateCounter = modulationUpdateRate;
			}

			float outL = delayLineL[GetIndex(delaySamplesL)];
			float outR = delayLineR[GetIndex(delaySamplesR)];

			delayLineL[samplePos] = Utils::QuickNonlin(inputL[i] + outL * totalFeedbackL);
			delayLineR[samplePos] = Utils::QuickNonlin(inputR[i] + outR * totalFeedbackR);

			samplePos--;
			if (samplePos < 0)
				samplePos += delayBufferSize;

			bufferL[i] = inputL[i] + Wet * outL;
			bufferR[i] = inputR[i] + Wet * outR;
			updateCounter--;
		}
	}

	float* Delay::GetOutputL()
	{
		return bufferL;
	}

	float* Delay::GetOutputR()
	{
		return bufferR;
	}

	void Delay::Update()
	{
		delaySamplesL = (float)ValueTables::Get(Utils::Limit(DelayL + DelayLMod, 0, 1), AudioLib::ValueTables::Response3Dec) * samplerate;
		delaySamplesR = (float)ValueTables::Get(Utils::Limit(DelayR + DelayRMod, 0, 1), AudioLib::ValueTables::Response3Dec) * samplerate;
		totalFeedbackL = Utils::Limit(FeedbackL + FeedbackLMod, 0, 2);
		totalFeedbackR = Utils::Limit(FeedbackR + FeedbackRMod, 0, 2);
		totalSaturate = Utils::Limit(Saturate + SaturateMod, 0, 1);
	}

	inline int Delay::GetIndex(int offset)
	{
		int idx = samplePos + offset;
		idx = idx - (idx > delayBufferSize) * delayBufferSize;
		return idx;
	}
}
