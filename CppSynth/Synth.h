#ifndef LEIFTUR_SYNTH
#define LEIFTUR_SYNTH

#include <thread>
#include <string>

#include "Default.h"
#include "Voice.h"
#include "Osc/UdpTranceiver.h"
#include "Osc/OscMessage.h"
#include "Parameters.h"
#include "PresetManager.h"
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
		std::map<int, std::string> formattedParameters;
		volatile bool isClosing;
		PresetManager presetManager;
		Preset currentPreset;
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
		void SetParameter(int key, double value);
		void ProcessMidi(uint8_t* message);
		void ProcessAudio(float** buffer, int bufferSize);

		inline static void UnpackParameter(const int key, Module* module, int* parameter)
		{
			// Parameter values is interpreseted as
			// mmmmmmmmmmmmmmmmpppppppppppppppp
			// Upper 16 bits = module
			// Lower 16 bits = parameter
			*module = (Module)((key & 0xFFFF0000) >> 16);
			*parameter = key & 0x0000FFFF;
		}

		inline static constexpr int PackParameter(const Module module, const int parameter)
		{
			return (((int)module) << 16) | parameter;
		}

	private:

		void MessageListener();
		void LoadPreset(Preset preset);
		void HandleControlMessage(OscMessage msg);
		void SendStateToEditor();
		void SetParameterInner(Module module, int parameter, double value);
		std::string FormatParameter(Module module, int parameter, double value);
		void SendBackParameter(Module module, int parameter);
		void SetGlobalVoiceParameter(VoiceParameters parameter, double value);

		void NoteOn(char note, float velocity);
		void NoteOff(char note);
		void MidiCC(uint8_t byte1, uint8_t byte2);
		void MidiProgram(uint8_t program);
		void PitchWheel(float pitchbend);
		void SetModWheel(float value);
		void SetKeyPressure(int note, float pressure);
		void SetChannelPressure(float pressure);

		void UpdateVoiceStates();
		int FindNextMonoNote();		
	};
}

#endif
