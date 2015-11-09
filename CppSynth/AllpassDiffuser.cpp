#include "AllpassDiffuser.h"
#include "AudioLib/Utils.h"
#include "Default.h"

using namespace AudioLib;

namespace Leiftur
{
	SchroederAllpass::SchroederAllpass()
	{
		delayBuffer = 0;
		output = 0;
	}

	SchroederAllpass::~SchroederAllpass()
	{
		delete delayBuffer;
		delete output;
	}

	void SchroederAllpass::Initialize(int bufferSize)
	{
		this->bufferSize = bufferSize;
		delayBuffer = new float[DelayBufferSamples];
		output = new float[bufferSize];

		for (int i = 0; i < DelayBufferSamples; i++)
			delayBuffer[i] = 0.0;
		
		for (int i = 0; i < bufferSize; i++)
			output[i] = 0.0;
	}

	float* SchroederAllpass::GetOutput()
	{
		return output;
	}

	void SchroederAllpass::ClearBuffers()
	{
		Utils::ZeroBuffer(delayBuffer, DelayBufferSamples);
		Utils::ZeroBuffer(output, bufferSize);
	}

	__inline_always float SchroederAllpass::Process(float input)
	{
		auto delayedIndex = index - SampleDelay;
		if (delayedIndex < 0) delayedIndex += DelayBufferSamples;

		auto bufOut = delayBuffer[delayedIndex];
		auto inVal = input + bufOut * Feedback;

		delayBuffer[index] = inVal;
		float output = bufOut - inVal * Feedback;

		index++;
		if (index >= DelayBufferSamples) index -= DelayBufferSamples;
		return output;
	}

	void SchroederAllpass::Process(float* input, int sampleCount)
	{
		auto delayedIndex = index - SampleDelay;
		if (delayedIndex < 0) delayedIndex += DelayBufferSamples;

		for (int i = 0; i < sampleCount; i++)
		{
			auto bufOut = delayBuffer[delayedIndex];
			auto inVal = input[i] + bufOut * Feedback;

			delayBuffer[index] = inVal;
			output[i] = bufOut - inVal * Feedback;

			index++;
			delayedIndex++;
			if (index >= DelayBufferSamples) index -= DelayBufferSamples;
			if (delayedIndex >= DelayBufferSamples) delayedIndex -= DelayBufferSamples;
		}
	}

	float SchroederAllpass::Get(int delay)
	{
		return 0.0f;
	}


	// -----------------------------------------------------------------------------------------
	// -----------------------------------------------------------------------------------------
	// -----------------------------------------------------------------------------------------

	AllpassDiffuser::AllpassDiffuser()
	{
		UpdateParameters(1000.0f, 0.7);
	}

	AllpassDiffuser::~AllpassDiffuser()
	{
	}

	void AllpassDiffuser::Initialize(int bufferSize, int samplerate)
	{
		for (int i = 0; i < StageCount; i++)
		{
			filters[i].Initialize(bufferSize);
		}
	}

	float* AllpassDiffuser::GetOutput()
	{
		return filters[StageCount - 1].GetOutput();
	}

	float AllpassDiffuser::Process(float value)
	{
		value = filters[0].Process(value);
		value = filters[1].Process(value);
		value = filters[2].Process(value);
		value = filters[3].Process(value);
		value = filters[4].Process(value);
		value = filters[5].Process(value);
		value = filters[6].Process(value);
		value = filters[7].Process(value);
		return value;
	}

	void AllpassDiffuser::Process(float* input, int sampleCount)
	{
		filters[0].Process(input, sampleCount);

		for (int i = 1; i < StageCount; i++)
		{
			filters[i].Process(filters[i - 1].GetOutput(), sampleCount);
		}
	}

	void AllpassDiffuser::ClearBuffers()
	{
		for (int i = 0; i < StageCount; i++)
			filters[i].ClearBuffers();
	}

	void AllpassDiffuser::UpdateParameters(float sampleDelay, float feedback)
	{
		for (int i = 0; i < StageCount; i++)
		{
			float del = sampleDelay * (0.5 + random.NextFloat());
			
			if (del > SchroederAllpass::DelayBufferSamples)
				del = SchroederAllpass::DelayBufferSamples - 10;
			else if (del < 10)
				del = 10;

			filters[i].SampleDelay = (int)del;
			filters[i].Feedback = feedback;
		}
	}
}
