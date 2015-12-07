#include "Delay.h"
#include "AudioLib/Utils.h"
#include "AudioLib/ValueTables.h"

using namespace AudioLib;

namespace Polyhedrus
{
	Delay::Delay()
	{
		diffuseAmount = 0;
		delayL = 0;
		delayR = 0;
		feedbackL = 0;
		feedbackR = 0;
		highpass = 0;
		lowpass = 0;
		saturate = 0;
		sync = false;
		wet = 0;
		IsEnabled = true;
		
		bufferL = 0;
		bufferR = 0;
		delayLineL = 0;
		delayLineR = 0;
		samplerate = 48000;
		T = 1.0f / 48000.0f;
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

	void Delay::Initialize(int samplerate, int bufferSize)
	{
		delayBufferSize = samplerate * 2;
		bufferL = new float[bufferSize]();
		bufferR = new float[bufferSize]();
		delayLineL = new float[delayBufferSize]();
		delayLineR = new float[delayBufferSize]();
		this->samplerate = samplerate;
		this->T = 1.0f / samplerate;

		diffuserL.Initialize(bufferSize, samplerate);
		diffuserR.Initialize(bufferSize, samplerate);
		
		// I call this only on diffuserL so that the random number generators are out of sync
		// this gives different diffusion on each channel, widening the sound
		diffuserL.UpdateParameters(1000.0f, 0.7f);
	}

	void Delay::SetParameter(DelayParameters parameter, double value)
	{
		switch (parameter)
		{
		case DelayParameters::DiffuseAmount:
			diffuseAmount = (float)value;
			break;
		case DelayParameters::DiffuseSize:
			diffuserL.UpdateParameters((float)(value * samplerate * 0.025), 0.7f);
			diffuserR.UpdateParameters((float)(value * samplerate * 0.025), 0.7f);
			break;
		case DelayParameters::DelayL:
			delayL = (float)value;
			break;
		case DelayParameters::DelayR:
			delayR = (float)value;
			break;
		case DelayParameters::FeedbackL:
			feedbackL = (float)value;
			break;
		case DelayParameters::FeedbackR:
			feedbackR = (float)value;
			break;
		case DelayParameters::Highpass:
			highpass = (float)value;
			break;
		case DelayParameters::Lowpass:
			lowpass = (float)value;
			break;
		case DelayParameters::Saturate:
			saturate = (float)value;
			break;
		case DelayParameters::Sync:
			sync = value >= 0.5;
			break;
		case DelayParameters::Wet:
			wet = (float)value;
			break;
		}

		Update();
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

	void Delay::ClearBuffers()
	{
		for (int i = 0; i < delayBufferSize; i++)
		{
			delayLineL[i] = 0.0f;
			delayLineR[i] = 0.0f;
		}
	}

	void Delay::Update()
	{
		wetGain = Utils::Limit(2 * wet, 0.0f, 1.0f);
		dryGain = Utils::Limit(2 - 2 * wet, 0.0f, 1.0f);

		if (sync)
		{
			delayQL = (Quantization)(int)(Utils::Limit(delayL, 0.0f, 1.0f) * ((int)AudioLib::Quantization::_1d + 0.999f));
			delayQR = (Quantization)(int)(Utils::Limit(delayR, 0.0f, 1.0f) * ((int)AudioLib::Quantization::_1d + 0.999f));
			delaySamplesL = (int)TempoSync::GetSamplesPerNote(delayQL, Bpm, samplerate);
			delaySamplesR = (int)TempoSync::GetSamplesPerNote(delayQR, Bpm, samplerate);
			if (delaySamplesL >= delayBufferSize)
				delaySamplesL = delayBufferSize - 1;
			if (delaySamplesR >= delayBufferSize)
				delaySamplesR = delayBufferSize - 1;
		}
		else
		{
			delayTimeL = ValueTables::Get(Utils::Limit(delayL, 0.001f, 1.0f), AudioLib::ValueTables::Response2Oct);
			delayTimeR = ValueTables::Get(Utils::Limit(delayR, 0.001f, 1.0f), AudioLib::ValueTables::Response2Oct);
			delaySamplesL = (int)(delayTimeL * samplerate);
			delaySamplesR = (int)(delayTimeR * samplerate);
		}

		totalFeedbackL = Utils::Limit(feedbackL, 0.0f, 1.1f);
		totalFeedbackR = Utils::Limit(feedbackR, 0.0f, 1.1f);
		totalSaturate = Utils::Limit(saturate, 0, 1);
		satInner = 0.1f + totalSaturate * 1.9f;
		satOuter = satInner < 1 ? 1.0f / satInner : 1.0f;

		lpL.SetFc((float)ValueTables::Get(Utils::Limit(lowpass, 0.01f, 1.0f), AudioLib::ValueTables::Response4Oct));
		lpR.SetFc((float)ValueTables::Get(Utils::Limit(lowpass, 0.01f, 1.0f), AudioLib::ValueTables::Response4Oct));
		hpL.SetFc((float)ValueTables::Get(Utils::Limit(highpass, 0.0f, 0.99f), AudioLib::ValueTables::Response4Oct) * 0.25f);
		hpR.SetFc((float)ValueTables::Get(Utils::Limit(highpass, 0.0f, 0.99f), AudioLib::ValueTables::Response4Oct) * 0.25f);
	}

	inline int Delay::GetIndex(int offset)
	{
		int idx = samplePos + offset;
		idx = idx % delayBufferSize;
		return idx;
	}
}
