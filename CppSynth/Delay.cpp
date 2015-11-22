#include "Delay.h"
#include "AudioLib/Utils.h"
#include "AudioLib/ValueTables.h"

using namespace AudioLib;

namespace Leiftur
{
	Delay::Delay()
	{
		DiffuseAmount = 0;
		DelayL = 0;
		DelayR = 0;
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
		T = 1.0f / 48000.0f;
		modulationUpdateRate = 8;
		updateCounter = 0;
		samplePos = 0;

		delaySamplesL = 0;
		delaySamplesR = 0;
		totalFeedbackL = 0;
		totalFeedbackR = 0;
		totalSaturate = 0;
		delayBufferSize = 0;
		satInner = 1.0f;
		satOuter = 1.0f;
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
		this->T = 1.0f / samplerate;

		diffuserL.Initialize(bufferSize, samplerate);
		diffuserR.Initialize(bufferSize, samplerate);
		
		// I call this only on diffuserL so that the random number generators are out of sync
		// this gives different diffusion on each channel, widening the sound
		diffuserL.UpdateParameters(1000.0f, 0.7f);

		for (int i = 0; i < delayBufferSize; i++)
		{
			delayLineL[i] = 0.0;
			delayLineR[i] = 0.0;
		}
	}

	void Delay::SetParameter(DelayParameters parameter, double value)
	{
		switch (parameter)
		{
		case DelayParameters::DiffuseAmount:
			DiffuseAmount = (float)value;
			break;
		case DelayParameters::DiffuseSize:
			diffuserL.UpdateParameters((float)(value * samplerate * 0.025), 0.7f);
			diffuserR.UpdateParameters((float)(value * samplerate * 0.025), 0.7f);
			break;
		case DelayParameters::DelayL:
			DelayL = (float)value;
			break;
		case DelayParameters::DelayR:
			DelayR = (float)value;
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

			outL = hpL.Process(outL);
			outL = lpL.Process(outL);
			
			outR = hpR.Process(outR);
			outR = lpR.Process(outR);

			//outL = outL * (1 - DiffuseAmount) + diffuserL.Process(outL) * DiffuseAmount;
			//outR = outR * (1 - DiffuseAmount) + diffuserR.Process(outR) * DiffuseAmount;
			
			delayLineL[samplePos] = Utils::QuickNonlin(satInner * (inputL[i] + outL * totalFeedbackL)) * satOuter;
			delayLineR[samplePos] = Utils::QuickNonlin(satInner * (inputR[i] + outR * totalFeedbackR)) * satOuter;

			samplePos--;
			if (samplePos < 0)
				samplePos += delayBufferSize;

			bufferL[i] = dryGain * inputL[i] + wetGain * outL;
			bufferR[i] = dryGain * inputR[i] + wetGain * outR;
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
		wetGain = Utils::Limit(2 * Wet, 0.0f, 1.0f);
		dryGain = Utils::Limit(2 - 2 * Wet, 0.0f, 1.0f);

		delaySamplesL = (float)ValueTables::Get(Utils::Limit(DelayL + DelayLMod, 0, 1), AudioLib::ValueTables::Response2Dec) * samplerate;
		delaySamplesR = (float)ValueTables::Get(Utils::Limit(DelayR + DelayRMod, 0, 1), AudioLib::ValueTables::Response2Dec) * samplerate;
		totalFeedbackL = Utils::Limit(FeedbackL + FeedbackLMod, 0, 1.1);
		totalFeedbackR = Utils::Limit(FeedbackR + FeedbackRMod, 0, 1.1);
		totalSaturate = Utils::Limit(Saturate + SaturateMod, 0, 1);
		satInner = 0.1f + totalSaturate * 1.9f;
		satOuter = satInner < 1 ? 1.0f / satInner : 1.0f;

		lpL.SetFc((float)ValueTables::Get(Utils::Limit(Lowpass + LowpassMod, 0, 1), AudioLib::ValueTables::Response4Oct) * 0.5);
		lpR.SetFc((float)ValueTables::Get(Utils::Limit(Lowpass + LowpassMod, 0, 1), AudioLib::ValueTables::Response4Oct) * 0.5);
		hpL.SetFc((float)ValueTables::Get(Utils::Limit(Highpass + HighpassMod, 0, 1), AudioLib::ValueTables::Response4Oct) * 0.5);
		hpR.SetFc((float)ValueTables::Get(Utils::Limit(Highpass + HighpassMod, 0, 1), AudioLib::ValueTables::Response4Oct) * 0.5);
	}

	inline int Delay::GetIndex(int offset)
	{
		int idx = samplePos + offset;
		idx = idx % delayBufferSize;
		return idx;
	}
}
