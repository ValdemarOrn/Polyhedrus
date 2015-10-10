#ifndef OSCILLATOR
#define OSCILLATOR

#include "Default.h"
#include "Wavetable.h"

struct Oscillator
{
public:
	int Note;
	float PitchBend;
	float PitchMod;
	float WaveIndex;

private:
	Wavetable* wavetable;
	int samplerate;
	uint32_t increment;
	uint32_t iterator;
	int updateCounter;
	int shiftValue;
	float waveMix;
	float* waveA;
	float* waveB;

public:
	Oscillator();
	void Initialize(int samplerate);
	void SetWavetable(int table);
	void Update();
	void GetSamples(float* buffer, int count);
	
};


#endif
