#ifndef LEIFTUR_VOICE
#define LEIFTUR_VOICE

#include "Parameters.h"
#include "Oscillator.h"
#include "FilterHp.h"
#include "FilterMain.h"
#include "Vca.h"
#include "Envelope.h"
#include "ModMatrix.h"
#include "MixerSettings.h"

namespace Leiftur
{
	class Voice
	{
	public:
		int Note;
		bool Gate;

	private:
		ModMatrix modMatrix;
		MixerSettings mixer;

		Oscillator osc1;
		Oscillator osc2;
		Oscillator osc3;

		FilterHp hpFilterL;
		FilterHp hpFilterR;
		FilterMain mainFilterL;
		FilterMain mainFilterR;
		
		Vca vcaOsc1;
		Vca vcaOsc2;
		Vca vcaOsc3;
		Vca vcaOutputL;
		Vca vcaOutputR;

		Envelope ampEnv;
		Envelope filterEnv;

		float* oscMixL;
		float* oscMixR;

		float* outputL;
		float* outputR;
		float* output[2];

		int samplerate;
		int modulationUpdateRate;
		float velocity;

	public:
		Voice();
		~Voice();
		void Initialize(int samplerate, int modulationUpdateRate, int bufferSize);
		void SetParameter(Module module, int parameter, double value);
		void SetGate(float velocity);
		void TurnOff();
		void SetNote(int note);
		void SetPitchWheel(float pitchbend);
		void SetModWheel(float value);
		void SetKeyPressure(float value);
		void SetChannelPressure(float value);
		void Process(int bufferSize);
		float** GetOutput();
	private:
		void ProcessModulation();
		void MixOscillators(int bufferSize);

		void SetOscParameter(Module module, OscParameters parameter, double value);
		void SetMixerParameter(Module module, MixerParameters parameter, double value);
		void SetFilterHpParameter(Module module, FilterHpParameters parameter, double value);
		void SetFilterMainParameter(Module module, FilterMainParameters parameter, double value);
		void SetEnvParameter(Module module, EnvParameters parameter, double value);
	};
}

#endif