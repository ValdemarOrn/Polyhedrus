#ifndef LEIFTUR_VOICE
#define LEIFTUR_VOICE

#include "Parameters.h"
#include "Oscillator.h"
#include "FilterHp.h"
#include "FilterMain.h"
#include "Vca.h"
#include "Envelope.h"

namespace Leiftur
{
	class Voice
	{
	public:
		int Samplerate;
		int Note;
		float Velocity;

	private:
		Oscillator osc1;
		FilterHp hpFilter;
		FilterMain mainFilter;
		Vca vca;
		Envelope ampEnv;

	public:
		Voice();
		~Voice();
		void Initialize(int samplerate, int modulationUpdateRate, int bufferSize);
		void SetParameter(Module module, int parameter, double value);
		void SetGate(float gate);
		void SetNote(int note);
		void SetPitchWheel(float pitchbend);
		void Process(float* buffer, int bufferSize);
	};
}

#endif