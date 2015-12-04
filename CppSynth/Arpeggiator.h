#ifndef LEIFTUR_ARPEGGIATOR
#define LEIFTUR_ARPEGGIATOR

#include <stdint.h>
#include <vector>
#include "Parameters.h"
#include "VoiceAllocator.h"

namespace Leiftur
{
	enum class ArpPattern
	{
		Up = 0,
		Down = 1,
		UpDown1 = 2,
		UpDown2 = 3,
		DownUp1 = 4,
		DownUp2 = 5
	};

	class Arpeggiator
	{
	public:
		int Range;
		ArpPattern NotePtn;
		ArpPattern OctavePtn;
		float Gate;
		float Divide;
		int Bpm;
		bool Sync;

	private:
		bool isEnabled;
		int samplerate;
		VoiceAllocator* voiceAllocator;
		double notePhase;

		bool heldNotes[128];
		bool playingNotes[128];
		int currentNote;

		// stores a precomputed pattern for the entire arp sequence
		vector<int> pattern;
		int patternIndex;
		
	public:
		Arpeggiator();
		~Arpeggiator();
		void Initialize(int samplerate, VoiceAllocator* voiceAllocator);
		void SetParameter(ArpParameters parameter, double value);
		void Process(int len);

		void SetEnabled(bool isEnabled);
		void NoteOn(uint8_t note, float velocity);
		void NoteOff(uint8_t note);

	private:
		void ComputeFullPattern();
		vector<int> GetOctPattern();
		vector<int> Arpeggiator::MakePattern(ArpPattern pattern, vector<int> input);
	};
}

#endif

