#ifndef POLYHEDRUS_ENVELOPE
#define POLYHEDRUS_ENVELOPE

#include <memory>

#include "Parameters.h"
#include "AudioLib/ValueTables.h"

using AudioLib::ValueTables;

namespace Polyhedrus
{
	class Envelope
	{
	public:
		static const int CurveTableSize = 200;

		static const int SectionAttack = 0;
		static const int SectionHold = 1;
		static const int SectionDecay = 2;
		static const int SectionSustain = 3;
		static const int SectionRelease = 4;
		static const int SectionPostRelease = 5;

		static const float MaxTimeSeconds; // defined in body
		
		static inline float GetDecayTime(float input)
		{
			return (float)ValueTables::Get(input, ValueTables::Response3Dec) * MaxTimeSeconds;
		}
		
		bool Retrigger;
		float VelocityAmount;
		
	private:

		float attackCurve[CurveTableSize];
		float decayCurve[CurveTableSize];
		float releaseCurve[CurveTableSize];
		float velocityCurve[CurveTableSize];

		float velocity;
		bool gate;
		float attack;
		float hold;
		float decay;
		float release;

		int samplerate;
		int section;
		float iterator;
		float attackInc;
		float holdInc;
		float decayInc;
		float sustain;
		float releaseInc;
		float output;

		float attackLevel;
		float releaseLevel;

		inline float Lookup(float* table, float phase)
		{
			float idx = (float)(phase * (CurveTableSize - 0.001));
			int x0 = (int)idx;
			int x1 = x0 + 1;
			if (x1 >= CurveTableSize) x1 = CurveTableSize - 1;
			float mix = idx - x0;

			float value = table[x0] * (1 - mix) + table[x1] * mix;
			return value;
		}

	public:
		Envelope();
		~Envelope();
		void Initialize(int samplerate);
		void SetParameter(EnvParameters parameter, double value);
		float Process(int samples);
		inline float GetOutput() { return output * (1 - VelocityAmount + velocity * VelocityAmount); }
		void SetGate(bool gate, float velocity);
		void Silence();
		void Reset();
		std::vector<uint8_t> GetVisual();
		std::vector<uint8_t> GetVelocityVisual();
		void CreateCurve(float* table, double expo);
	};
}

#endif
