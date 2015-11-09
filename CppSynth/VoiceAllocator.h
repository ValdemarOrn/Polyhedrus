#ifndef LEIFTUR_VOICE_ALLOCATOR
#define LEIFTUR_VOICE_ALLOCATOR

#include "SynthDefines.h"
#include "Voice.h"

namespace Leiftur
{
	class VoiceAllocator
	{
	public:
		VoiceMode voiceMode;
		int polyphony;
		int unison;

		VoiceAllocator();
		void Initialize(Voice* voices);
		void UpdateVoiceStates();
		void NoteOn(uint8_t note, float velocity);
		void NoteOff(uint8_t note);

		inline int GetVoiceCount()
		{
			return effectivePolyphony;
		}

	private:
		Voice* voices;
		float currentVelocity;

		int effectivePolyphony;
		int effectiveUnison;
		VoiceMode effectiveVoiceMode;
		int lastAllocatedVoice;

		int noteCounters[128]; // used to track newest/oldest notes in mono mode
		int voiceCounters[128]; // used to track least recently triggered voice in poly mode

		int noteCounter;
		int voiceCounter;

		int FindNextMonoNote();
		int FindNextVoice();
	};
}

#endif
