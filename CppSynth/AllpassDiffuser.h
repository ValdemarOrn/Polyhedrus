#ifndef POLYHEDRUS_ALLPASS_DIFFUSER
#define POLYHEDRUS_ALLPASS_DIFFUSER

#include "AudioLib/LcgRandom.h"

namespace Polyhedrus
{
	class SchroederAllpass
	{
	public:
		static const int DelayBufferSamples = 9600; // 50ms at 192Khz

	private:
		float* delayBuffer;
		float* output;
		int bufferSize;
		int index;
	public:

		int SampleDelay;
		float Feedback;

		SchroederAllpass();
		~SchroederAllpass();
		void Initialize(int bufferSize);

		float* GetOutput();
		void ClearBuffers();
		float Process(float input);
		void Process(float* input, int sampleCount);
	private:
		float Get(int delay);
	};

	class AllpassDiffuser
	{
	public:
		static const int StageCount = 8;

	private:
		int samplerate;
		SchroederAllpass filters[StageCount];
		AudioLib::LcgRandom random;
		int delay;

	public:
		AllpassDiffuser();
		~AllpassDiffuser();
		void Initialize(int bufferSize, int samplerate);
		float* GetOutput();

		float Process(float input);
		void Process(float* input, int sampleCount);
		void ClearBuffers();
		void UpdateParameters(float sampleDelay, float feedback);
	};
}

#endif