#ifndef LEIFTUR_ARPEGGIATOR
#define LEIFTUR_ARPEGGIATOR

#include <stdint.h>
#include "Parameters.h"
#include "VoiceAllocator.h"

namespace Leiftur
{
	class Arpeggiator
	{
	public:
		bool IsEnabled;

		int Range;
		int NotePtn;
		int OctavePtn;
		float Gate;
		float Divide;
		int Bpm;
		bool Sync;

	private:
		int samplerate;
		VoiceAllocator* voiceAllocator;
		double notePhase;

		bool heldNotes[128];
		bool playingNotes[128];
		int currentNote;

		// stores a precomputed pattern for the entire arp sequence
		uint8_t pattern[100];
		int patternIndex;
		int patternLen;
		
	public:
		Arpeggiator();
		~Arpeggiator();
		void Initialize(int samplerate, VoiceAllocator* voiceAllocator);
		void SetParameter(ArpParameters parameter, double value);
		void Process(int len);

		void NoteOn(uint8_t note, float velocity);
		void NoteOff(uint8_t note);

	private:
		void ComputePattern();
	};
}

#endif

