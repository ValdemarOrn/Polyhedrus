#ifndef POLYHEDRUS_SYNTH
#define POLYHEDRUS_SYNTH

#include <thread>
#include <string>

#include "Default.h"
#include "Voice.h"
#include "Osc/UdpTranceiver.h"
#include "Osc/OscMessage.h"
#include "Parameters.h"
#include "PresetManager.h"
#include "SynthDefines.h"
#include "VoiceAllocator.h"
#include "Delay.h"
#include "AudioLib/Decimator.h"
#include "Arpeggiator.h"

#include <map>
#include <memory>

using std::shared_ptr;

namespace Polyhedrus
{
	class Synth
	{
	public:
		Voice Voices[MaxVoiceCount];
		vector<uint8_t> VoiceStates;
		Delay Delay;

	private:
		int samplerate;
		int oversampling;
		bool moduleSwitches[20];

		std::map<int, std::string> formattedParameters;
		volatile bool isClosing;
		PresetManager presetManager;
		shared_ptr<WavetableManager> wavetableManager;
		Preset currentPreset;
		UdpTranceiver* udpTranceiver;
		std::thread messageListenerThread;
		VoiceAllocator voiceAllocator;
		Arpeggiator arpeggiator;
		AudioLib::Decimator decimatorL;
		AudioLib::Decimator decimatorR;
		float voiceTuningAmount[VoiceTuningCount];
		int voiceTuningSeeds[VoiceTuningCount];
		float* outputBufferL;
		float* outputBufferR;
		float masterVol;
		bool isReady;

	public:
		Synth();
		~Synth();

		void Initialize(int samplerate, bool oversample, int udpListenPort, int udpSendPort);
		void SetParameter(int key, double value);
		void ProcessMidi(uint8_t* message);
		void ProcessAudio(float** buffer, int bufferLen);

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
		void HandleControlMessage(OscMessage msg);
		void LoadPreset(std::string bank, std::string presetName);
		void LoadPreset(Preset preset);
		void SendStateToEditor();
		void SendWaveformsToEditor();
		void SendBanksToEditor();
		void SendPresetsToEditor(std::string bankName);
		void SavePreset(std::string bankName, std::string presetName);
		void SendVisual(Module module);
		void SendVoiceStates();
		void SetParameterInner(Module module, int parameter, double value);
		std::string FormatParameter(Module module, int parameter, double value);
		void SendBackParameter(Module module, int parameter);
		void SetGlobalVoiceParameter(VoiceParameters parameter, double value);
		void SetGlobalModuleSwitchParameter(ModuleSwitchParameters parameter, double value);
		void SetGlobalArpParameter(ArpParameters parameter, double value);
		void SetGlobalVoiceTuningParameter(VoiceTuningParameters parameter, double value);

		void NoteOn(uint8_t note, float velocity);
		void NoteOff(uint8_t note);
		void MidiCC(uint8_t byte1, uint8_t byte2);
		void MidiProgram(uint8_t program);
		void PitchWheel(float pitchbend);
		void SetModWheel(float value);
		void SetKeyPressure(int note, float pressure);
		void SetChannelPressure(float pressure);
	};
}

#endif
