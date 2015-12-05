#include <algorithm>
#include "Arpeggiator.h"

namespace Polyhedrus
{
	Arpeggiator::Arpeggiator()
	{
		isEnabled = false;
		Range = 3;
		NotePtn = ArpPattern::Up;
		OctavePtn = ArpPattern::Up;
		Gate = 0.8;
		Divide = 2.0f; // 8th notes
		Bpm = 120;
		Sync = false;
		currentNote = -1;
		patternIndex = 0;
		trigger = false;
		pattern.clear();
		heldNotes.clear();
		playingNotes.clear();

		ComputeFullPattern();
	}

	Arpeggiator::~Arpeggiator()
	{
	}

	void Arpeggiator::Initialize(int samplerate, VoiceAllocator* voiceAllocator)
	{
		this->samplerate = samplerate;
		this->voiceAllocator = voiceAllocator;
	}

	void Arpeggiator::SetParameter(ArpParameters parameter, double value)
	{
		switch (parameter)
		{
		case ArpParameters::Bpm:
			Bpm = Parameters::FloorToInt(value);
			break;
		case ArpParameters::Divide:
			Divide = value;
			break;
		case ArpParameters::Gate:
			Gate = (float)value;
			break;
		case ArpParameters::NotePtn:
			NotePtn = (ArpPattern)Parameters::FloorToInt(value);
			ComputeFullPattern();
			break;
		case ArpParameters::OctavePtn:
			OctavePtn = (ArpPattern)Parameters::FloorToInt(value);
			ComputeFullPattern();
			break;
		case ArpParameters::Range:
			Range = Parameters::FloorToInt(value);
			ComputeFullPattern();
			break;
		case ArpParameters::Sync:
			Sync = value >= 0.5;
			break;
		}
	}

	void Arpeggiator::Process(int len)
	{
		vector<int> patternCopy = pattern;

		if (!isEnabled)
			return;

		if (patternCopy.size() == 0)
			return; // no notes on, silence

		if (notePhase >= Gate)
		{
			voiceAllocator->NoteOff(currentNote);
			playingNotes.erase(currentNote);
		}

		if (trigger)
		{
			// when releasing notes, the pattern index should get reset explicitly, this is just a safeguard
			patternIndex = patternIndex % patternCopy.size();
			int noteToPlay = patternCopy.at(patternIndex);

			// silence old note
			if (currentNote != -1)
			{
				voiceAllocator->NoteOff(currentNote);
				playingNotes.erase(currentNote);
			}

			// play new note
			currentNote = noteToPlay;
			voiceAllocator->NoteOn(currentNote, 1.0f);
			playingNotes.insert(currentNote);

			trigger = false;
		}
		
		// increment phase
		double notesPerSecond = Bpm / 60.0 * (1 + Divide * 3);
		double samplesPerNote = samplerate / notesPerSecond;
		double phaseInc = 1.0 / samplesPerNote * len;
		notePhase += phaseInc;

		if (notePhase >= 1.0)
		{
			notePhase -= 1.0;
			trigger = true;

			// set new pattern index
			patternIndex = (patternIndex + 1) % patternCopy.size();
		}
	}

	void Arpeggiator::SetEnabled(bool isEnabled)
	{
		this->isEnabled = isEnabled;
		vector<int> notes;

		for (auto note : playingNotes)
			notes.push_back(note);

		for (auto note : notes)
		{
			voiceAllocator->NoteOff(note);
			playingNotes.erase(note);
		}

		notePhase = 1.0;
	}

	void Arpeggiator::NoteOn(uint8_t note, float velocity)
	{
		heldNotes.insert(note);
		ComputeFullPattern();

		// if no notes are playing, we start the pattern immediately
		if (isEnabled && currentNote == -1)
		{
			patternIndex = 0;
			notePhase = 0.0;
			trigger = true;
		}
		else if (!isEnabled)
		{
			voiceAllocator->NoteOn(note, velocity);
			playingNotes.insert(note);
		}
	}

	void Arpeggiator::NoteOff(uint8_t note)
	{
		heldNotes.erase(note);
		ComputeFullPattern();

		// if no notes are held, we stop the pattern immediately
		if (isEnabled && pattern.size() == 0)
		{
			voiceAllocator->NoteOff(currentNote);
			playingNotes.erase(currentNote);
			currentNote = -1;
			trigger = false;
		}
		else if (!isEnabled)
		{
			voiceAllocator->NoteOff(note);
			playingNotes.erase(note);
		}
	}

	void Arpeggiator::ComputeFullPattern()
	{
		vector<int> notes;

		for (auto note : heldNotes)
			notes.push_back(note);

		if (notes.size() == 0)
		{
			pattern.clear();
			return;
		}
		
		auto octPtn = GetOctPattern();
		auto notePtn = MakePattern(NotePtn, notes);
		int patternLen = 0;
		vector<int> newPattern;

		for (int i = 0; i < octPtn.size(); i++)
		{
			int oct = octPtn[i];
			for (int j = 0; j < notePtn.size(); j++)
			{
				newPattern.push_back(notePtn[j] + oct * 12);
			}
		}

		int newIndex = RecomputeCurrentIndex(newPattern);
		
		patternIndex = newIndex;
		pattern = newPattern;
	}

	vector<int> Arpeggiator::GetOctPattern()
	{
		vector<int> octs;
		for (int i = 0; i < Range; i++)
		{
			octs.push_back(i);
		}

		return MakePattern(OctavePtn, octs);
	}

	vector<int> Arpeggiator::MakePattern(ArpPattern pattern, vector<int>& input)
	{
		std::sort(input.begin(), input.end());

		if (input.size() == 1)
			return input;

		if (pattern == ArpPattern::Up)
		{
			return input;
		}
		else if (pattern == ArpPattern::Down)
		{
			vector<int> output;
			for (int i = input.size() - 1; i >= 0; i--)
				output.push_back(input[i]);

			return output;
		}
		else if (pattern == ArpPattern::UpDown1)
		{
			vector<int> output;

			for (int i = 0; i < input.size(); i++)
				output.push_back(input[i]);

			for (int i = input.size() - 2; i > 0; i--)
				output.push_back(input[i]);

			return output;
		}
		else if (pattern == ArpPattern::UpDown2)
		{
			vector<int> output;

			for (int i = 0; i < input.size(); i++)
				output.push_back(input[i]);

			for (int i = input.size() - 1; i >= 0; i--)
				output.push_back(input[i]);

			return output;
		}
		else if (pattern == ArpPattern::DownUp1)
		{
			vector<int> output;

			for (int i = input.size() - 1; i >= 0; i--)
				output.push_back(input[i]);

			for (int i = 1; i < input.size() - 1; i++)
				output.push_back(input[i]);
			
			return output;
		}
		else if (pattern == ArpPattern::DownUp2)
		{
			vector<int> output;

			for (int i = input.size() - 1; i >= 0; i--)
				output.push_back(input[i]);

			for (int i = 0; i < input.size(); i++)
				output.push_back(input[i]);

			return output;
		}

		// should never happen, but just in case. better than crashing
		return input;
	}
	
	int Arpeggiator::RecomputeCurrentIndex(const vector<int>& newPattern)
	{
		vector<int> possibleIndices;

		for (int i = 0; i < newPattern.size(); i++)
		{
			if (currentNote == newPattern[i])
				possibleIndices.push_back(i);
		}

		// the current note is no longer in the pattern
		if (possibleIndices.size() == 0)
			return patternIndex % newPattern.size();
		
		int minDist = 9999;
		int bestIndex = -1;

		// set the new index to be the possible new index that is closest to the current one.
		for (auto newIndex : possibleIndices)
		{
			int dist = std::abs(newIndex - patternIndex);
			if (dist < minDist)
			{
				minDist = dist;
				bestIndex = newIndex;
			}
		}

		return bestIndex;
	}
}
