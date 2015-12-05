#ifndef POLYHEDRUS_ARPEGGIATOR
#define POLYHEDRUS_ARPEGGIATOR

#include <stdint.h>
#include <vector>
#include <unordered_set>
#include "Parameters.h"
#include "VoiceAllocator.h"

namespace Polyhedrus
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

		std::unordered_set<int> heldNotes;
		std::unordered_set<int> playingNotes;
		int currentNote;
		bool trigger;

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
		vector<int> Arpeggiator::MakePattern(ArpPattern pattern, vector<int>& input);
		int RecomputeCurrentIndex(const vector<int>& newPattern);
	};
}

#endif

