#ifndef POLYHEDRUS_VOICE
#define POLYHEDRUS_VOICE

#include "NoiseSimple.h"
#include "Parameters.h"
#include "Oscillator.h"
#include "FilterHp.h"
#include "FilterMain.h"
#include "Vca.h"
#include "Envelope.h"
#include "ModMatrix.h"
#include "MixerSettings.h"
#include "Character.h"
#include "Modulator.h"
#include "Drive.h"
#include "AudioLib/SlopGenerator.h"
#include "SynthDefines.h"

namespace Polyhedrus
{
	class Voice
	{
	public:
		int VoiceNumber;
		int Note;
		bool Gate;

		shared_ptr<WavetableManager> wavetableManager;
		ModMatrix modMatrix;
		float voiceTuning[VoiceTuningCount];
		MixerSettings mixer;
		bool moduleSwitches[20];

		AudioLib::SlopGenerator slopGen1;
		AudioLib::SlopGenerator slopGen2;
		AudioLib::SlopGenerator slopGen3;
		Oscillator osc1;
		Oscillator osc2;
		Oscillator osc3;
		NoiseSimple noise;

		Character characterL;
		Character characterR;
		FilterHp hpFilterL;
		FilterHp hpFilterR;
		FilterMain mainFilterL;
		FilterMain mainFilterR;
		Drive driveL;
		Drive driveR;

		Vca vcaOsc1;
		Vca vcaOsc2;
		Vca vcaOsc3;
		Vca vcaOutputL;
		Vca vcaOutputR;

		Envelope ampEnv;
		Envelope filterEnv;
		Modulator mod1;
		Modulator mod2;
		Modulator mod3;

	private:
		float* signalMixL;
		float* signalMixR;

		float* osc1Buffer;
		float* osc2Buffer;
		float* osc3Buffer;
		float* outputL;
		float* outputR;
		float* output[2];

		int samplerate;
		int modulationUpdateRate;
		float velocity;

	public:
		Voice();
		~Voice();
		void Initialize(int samplerate, int modulationUpdateRate, int bufferSize, int voiceNumber, shared_ptr<WavetableManager> wavetableManager);
		void SetParameter(Module module, int parameter, double value);
		void SetGate(float velocity);
		void TurnOff();
		void SetNote(int note);
		void SetPitchWheel(float pitchbend);
		void SetModWheel(float value);
		void SetKeyPressure(float value);
		void SetChannelPressure(float value);
		void SetUnisonValue(float value);
		void Process(int bufferSize);
		float** GetOutput();
		inline int GetState()
		{
			if (this->Gate == true)
				return 2;
			return ampEnv.GetOutput() > 0 ? 1 : 0;
		}

	private:
		void ProcessModulation();
		void ProcessAm(int bufferSize);
		void MixSignals(int bufferSize, RoutingStage stage);
		bool IsEnabled(ModuleSwitchParameters module);

		void SetOscParameter(Module module, OscParameters parameter, double value);
		void SetMixerParameter(Module module, MixerParameters parameter, double value);
		void SetModuleSwitchParameter(Module module, ModuleSwitchParameters parameter, double value);
		void SetCharacterParameter(Module module, CharacterParameters parameter, double value);
		void SetFilterHpParameter(Module module, FilterHpParameters parameter, double value);
		void SetFilterMainParameter(Module module, FilterMainParameters parameter, double value);
		void SetDriveParameter(Module mdoule, DriveParameters parameter, double value);
		void SetEnvParameter(Module module, EnvParameters parameter, double value);
		void SetModParameter(Module module, ModParameters parameter, double value);
		void SetVoiceParameter(Module module, VoiceParameters parameter, double value);
		void SetModMatrixParameter(Module module, ModMatrixParameters parameter, double value);
	};
}

#endif