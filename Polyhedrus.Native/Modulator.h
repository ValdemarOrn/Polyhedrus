#ifndef POLYHEDRUS_MODULATOR
#define POLYHEDRUS_MODULATOR

#include "Envelope.h"
#include "Parameters.h"
#include "AudioLib/ValueTables.h"
#include "AudioLib/LcgRandom.h"

namespace Polyhedrus
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

	class Modulator
	{
	public:
		static const int MaxFreq = 400;

		bool IsEnabled;
		float Delay;
		float Attack;
		float Hold;
		float Decay;
		float Sustain;
		float Release;

		float DelayMod;
		float AttackMod;
		float HoldMod;
		float DecayMod;
		float SustainMod;
		float ReleaseMod;
		
		LfoShape Shape;

		float Phase;
		float Freq;
		float Slew;
		float Steps;
		bool Sync;

		float FreqMod;
		float SlewMod;
		float StepsMod;

		float Output;
		float OutputUnipolar;
		float OutputEnv;

	private:
		Envelope env;
		int samplerate;

		uint32_t iterator;
		uint32_t increment;
		bool gate;
		float slewPerSample;
		int stepCount;
		float prevLfoSample;
		float slewTotal;
	
		// for noise
		uint32_t prevIterator;
		float currentSh;
		AudioLib::LcgRandom random;

	public:
		Modulator();
		~Modulator();
		void Initialize(int samplerate);
		void SetParameter(ModParameters parameter, double value);
		float Process(int samples);
		void Reset();
		inline void SetGate(bool gate, float velocity) { env.SetGate(gate, velocity); }
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

