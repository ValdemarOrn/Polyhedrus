#ifndef LEIFTUR_SYNTH
#define LEIFTUR_SYNTH

#include <thread>
#include <string>

#include "Default.h"
#include "Voice.h"
#include "Osc/UdpTranceiver.h"
#include "Parameters.h"
#include <map>

namespace Leiftur
{
	const int MaxVoiceCount = 32;
	const int ModulationUpdateRate = 8;
	const int BufferSize = 64;

	enum class VoiceMode
	{
		MonoHighest = 0,
		MonoLowest = 1,
		MonoNewest = 2,
		PolyRoundRobin = 3,
	};

	class Synth
	{
	public:
		Voice Voices[MaxVoiceCount];
		int Samplerate;

	private:
		std::map<int, double> parameters;
		volatile bool isClosing;
		UdpTranceiver* udpTranceiver;
		std::thread messageListenerThread;
		VoiceMode voiceMode;
		int polyphony;
		int unison;
		int nextVoiceIndex;
		int noteCounters[128];
		int noteCounter;
		float lastVelocity;
		float masterVol;

	public:
		Synth();
		~Synth();

		void Initialize(int samplerate, int udpListenPort, int udpSendPort);

		void SetParameter(int parameter, double value);
		void SetParameter(std::string address, double value);
		void ProcessMidi(uint8_t* message);
		void ProcessAudio(float** buffer, int bufferSize);

	private:

		void MessageListener();
		void SetParameterInner(Module module, int parameter, double value, bool isTranslated);
		//double TranslateValue(Module module, int parameter, double value);

		void NoteOn(char note, float velocity);
		void NoteOff(char note);
		void MidiCC(uint8_t byte1, uint8_t byte2);
		void MidiProgram(uint8_t program);
		void PitchWheel(float pitchbend);
		void SetModWheel(float value);
		void SetKeyPressure(int note, float pressure);
		void SetChannelPressure(float pressure);

		void SetGlobalVoiceParameter(VoiceParameters parameter, double value);
		void UpdateVoiceStates();
		int FindNextMonoNote();
	};
}

#endif
