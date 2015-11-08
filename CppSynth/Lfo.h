#ifndef LEIFTUR_LFO
#define LEIFTUR_LFO

#include "Envelope.h"
#include "Parameters.h"
#include "AudioLib/ValueTables.h"

namespace Leiftur
{
	enum class LfoShape
	{
		Triangle = 0,
		Sine = 1,
		Saw = 2,
		Ramp = 3,
		Square = 4,
		Pulse1 = 5,
		Pulse2 = 6,
		SampleHold = 7,

		Count = 8
	};

	class Lfo
	{
	public:
		static const int MaxFreq = 100;
		float FreqMod;
		float PhaseMod;

	private:
		Envelope env;
		int samplerate;
		float output;

		uint32_t iterator;
		uint32_t increment;
		bool gate;
		float phase;
		float freq;
		bool sync;
		LfoShape shape;
	
		// for noise
		uint32_t updatedAt;
		int randIdx;

	public:
		Lfo();
		~Lfo();
		void Initialize(int samplerate);
		void SetParameter(ModParameters parameter, double value);
		float Process(int samples);
		inline float GetOutput() { return output; }
		void Reset();

		inline static float GetFrequency(float value)
		{
			return (float)(ValueTables::Get(value, ValueTables::Response4Dec) * MaxFreq);
		}

	private:
		void Update();
		float GetSample();
	};
}

#endif

