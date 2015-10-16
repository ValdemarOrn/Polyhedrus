#ifndef LEIFTUR_OSCILLATOR
#define LEIFTUR_OSCILLATOR

#include "Default.h"
#include "Wavetable.h"

namespace Leiftur
{
	struct Oscillator
	{
	public:
		int Note;
		int Octave;
		int Semi;
		int Cent;
		float PitchBend;
		float PitchMod;

		float Phase;
		float Shape;

	private:
		Wavetable* wavetable;
		int samplerate;
		int modulationUpdateRate;
		uint32_t increment;
		uint32_t iterator;
		int updateCounter;
		int shiftValue;
		float waveMix;
		float* waveA;
		float* waveB;

	public:
		Oscillator();
		void Initialize(int samplerate, int modulationUpdateRate);
		void SetWaveform(int table);
		void Reset();
		void GetSamples(float* buffer, int count);

	private:
		void Update();
	};
}

#endif
