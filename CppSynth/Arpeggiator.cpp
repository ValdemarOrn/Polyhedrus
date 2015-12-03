#include "Arpeggiator.h"

namespace Leiftur
{
	Arpeggiator::Arpeggiator()
	{
		IsEnabled = false;
		Range = 3;
		NotePtn = 1;
		OctavePtn = 1;
		Gate = 0.8;
		Divide = 2.0f; // 8th notes
		Bpm = 120;
		Sync = false;
		currentNote = -1;
		patternIndex = 0;
		patternLen = 0;

		for (size_t i = 0; i < 128; i++)
		{
			heldNotes[i] = false;
			playingNotes[i] = false;
		}

		for (size_t i = 0; i < 100; i++)
		{
			pattern[i] = 60;
		}
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
			NotePtn = Parameters::FloorToInt(value);
			break;
		case ArpParameters::OctavePtn:
			OctavePtn = Parameters::FloorToInt(value);
			break;
		case ArpParameters::Range:
			Range = Parameters::FloorToInt(value);
			break;
		case ArpParameters::Sync:
			Sync = value >= 0.5;
			break;
		}
	}

	void Arpeggiator::Process(int len)
	{
		if (!IsEnabled)
			return;

		double notesPerSecond = Bpm / 60.0 *  Divide;
		double samplesPerNote = samplerate / notesPerSecond;

		double phaseInc = 1.0 / samplesPerNote * len;
		notePhase += phaseInc;

		if (notePhase < 1.0)
			return; // noting to do

		notePhase -= 1.0;

		// silence old note
		if (currentNote != -1)
		{
			voiceAllocator->NoteOff(currentNote);
			playingNotes[currentNote] = false;
			currentNote = -1;
		}

		if (patternLen == 0)
			return; // no notes on, silence
		
		// set new pattern index
		patternIndex++;
		if (patternIndex >= patternLen)
			patternIndex = 0;
		
		currentNote = pattern[patternIndex];
		voiceAllocator->NoteOn(currentNote, 1.0f);
		playingNotes[currentNote] = true;
	}

	void Arpeggiator::NoteOn(uint8_t note, float velocity)
	{
		heldNotes[note] = true;
		ComputePattern();

		if (!IsEnabled)
		{
			voiceAllocator->NoteOn(note, velocity);
			playingNotes[note] = true;
		}
	}

	void Arpeggiator::NoteOff(uint8_t note)
	{
		heldNotes[note] = false;
		ComputePattern();

		if (!IsEnabled)
		{
			voiceAllocator->NoteOff(note);
			playingNotes[note] = false;
		}
	}

	void Arpeggiator::ComputePattern()
	{
		int note = -1;
		for (int i = 0; i < 128; i++)
		{
			if (heldNotes[i])
			{
				note = i;
				break;
			}
		}

		if (note == -1)
			patternLen = 0;
		else
		{
			pattern[0] = note;
			pattern[1] = note + 12;
			pattern[2] = note + 24;
			patternLen = 3;
		}
	}
}
