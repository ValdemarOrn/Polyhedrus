#ifndef VOICE
#define VOICE

#include "Oscillator.h"

class Voice
{
public:
	int Samplerate;
	int Note;
	float Velocity;

private:
	Oscillator osc1;

public:
	Voice();
	~Voice();
	void Initialize(int samplerate);
	void SetParameter(int parameter, double value);
	void SetGate(float gate);
	void SetNote(int note);
	void SetPitchWheel(float pitchbend);
	void Process(float* buffer, int bufferSize);
};

#endif