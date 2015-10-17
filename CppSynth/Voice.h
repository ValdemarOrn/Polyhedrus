#ifndef LEIFTUR_VOICE
#define LEIFTUR_VOICE

#include "Parameters.h"
#include "Oscillator.h"
#include "FilterHp.h"
#include "FilterMain.h"
#include "Vca.h"
#include "Envelope.h"
#include "ModMatrix.h"

namespace Leiftur
{
	class Voice
	{
	public:
		int Note;

	private:
		ModMatrix modMatrix;

		Oscillator osc1;
		Oscillator osc2;
		Oscillator osc3;

		FilterHp hpFilter;
		FilterMain mainFilter;
		
		Vca vcaOsc1;
		Vca vcaOsc2;
		Vca vcaOsc3;
		Vca vcaOutput;

		Envelope ampEnv;
		Envelope filterEnv;
		Envelope modEnv;

		int samplerate;
		int modulationUpdateRate;
		float velocity;

	public:
		Voice();
		~Voice();
		void Initialize(int samplerate, int modulationUpdateRate, int bufferSize);
		void SetParameter(Module module, int parameter, double value);
		void SetGate(float velocity);
		void SetNote(int note);
		void SetPitchWheel(float pitchbend);
		void SetModWheel(float value);
		void SetKeyPressure(float value);
		void SetChannelPressure(float value);
		void Process(float* buffer, int bufferSize);
	};
}

#endif