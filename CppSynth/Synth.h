#include "Default.h"
#include "Voice.h"

const int MaxVoiceCount = 1;

class Synth
{
public:
	Voice Voices[MaxVoiceCount];
	int Samplerate;

public:
	void Initialize(int samplerate);

	void SetParameter(int parameter, double value);
	void ProcessMidi(uint8_t* message);
	void ProcessAudio(float** buffer, int bufferSize);

private:
	void NoteOn(char note, char velocity);
	void NoteOff(char note);
	void MidiCC(uint8_t byte1, uint8_t byte2);
	void MidiProgram(uint8_t program);
	void PitchWheel(int pitchbend);
};