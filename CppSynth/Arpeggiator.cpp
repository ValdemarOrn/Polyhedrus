#include <algorithm>
#include "Arpeggiator.h"

namespace Leiftur
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
		pattern.clear();

		for (size_t i = 0; i < 128; i++)
		{
			heldNotes[i] = false;
			playingNotes[i] = false;
		}

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

		double notesPerSecond = Bpm / 60.0 * (1 + Divide * 3);
		double samplesPerNote = samplerate / notesPerSecond;
		double phaseInc = 1.0 / samplesPerNote * len;
		
		// when releasing notes, the pattern index should get reset explicitly, this is just a safeguard
		patternIndex = patternIndex % patternCopy.size();

		int noteToPlay = patternCopy.at(patternIndex);
		
		if (currentNote != noteToPlay)
		{
			// silence old note
			voiceAllocator->NoteOff(currentNote);
			playingNotes[currentNote] = false;
			
			// play new note
			currentNote = noteToPlay;
			voiceAllocator->NoteOn(currentNote, 1.0f);
			playingNotes[currentNote] = true;
		}
		
		// increment phase
		notePhase += phaseInc;
		if (notePhase >= 1.0)
		{
			notePhase -= 1.0;

			// set new pattern index
			patternIndex++;
			if (patternIndex >= patternCopy.size())
				patternIndex = 0;
		}
	}

	void Arpeggiator::SetEnabled(bool isEnabled)
	{
		this->isEnabled = isEnabled;
		for (int i = 0; i < 128; i++)
		{
			if (playingNotes[i])
			{
				voiceAllocator->NoteOff(i);
				playingNotes[i] = false;
			}
		}
		notePhase = 1.0;
	}

	void Arpeggiator::NoteOn(uint8_t note, float velocity)
	{
		heldNotes[note] = true;
		ComputeFullPattern();

		// if no notes are playing, we start the pattern immediately
		if (isEnabled && currentNote == -1)
		{
			patternIndex = 0;
			notePhase = 0.0;
		}
		else if (!isEnabled)
		{
			voiceAllocator->NoteOn(note, velocity);
			playingNotes[note] = true;
		}
	}

	void Arpeggiator::NoteOff(uint8_t note)
	{
		heldNotes[note] = false;
		ComputeFullPattern();

		// if no notes are held, we stop the pattern immediately
		if (isEnabled && pattern.size() == 0)
		{
			voiceAllocator->NoteOff(currentNote);
			playingNotes[currentNote] = false;
			currentNote = -1;
		}
		else if (!isEnabled)
		{
			voiceAllocator->NoteOff(note);
			playingNotes[note] = false;
		}
	}

	void Arpeggiator::ComputeFullPattern()
	{
		vector<int> notes;

		for (int i = 0; i < 128; i++)
		{
			if (heldNotes[i])
				notes.push_back(i);
		}

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

	vector<int> Arpeggiator::MakePattern(ArpPattern pattern, vector<int> input)
	{
		std::sort(input.begin(), input.end());

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
	}
}
