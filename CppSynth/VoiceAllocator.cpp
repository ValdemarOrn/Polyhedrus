#include "VoiceAllocator.h"

namespace Leiftur
{
	VoiceAllocator::VoiceAllocator()
	{
		polyphony = 1;
		unison = 1;
		currentVelocity = 0;
		lastAllocatedVoice = -1;

		for (size_t i = 0; i < 128; i++)
		{
			noteCounters[i] = 0;
			voiceCounters[i] = 0;
		}

		noteCounter = 1;
		voiceCounter = 1;
	}

	void VoiceAllocator::Initialize(Voice* voices)
	{
		this->voices = voices;
	}

	void VoiceAllocator::UpdateVoiceStates()
	{
		// when changing the voice mode, it's safest to just reset all the voices to prevent stuck notes
		bool resetAll = effectiveVoiceMode != voiceMode;

		for (int i = 0; i < MaxVoiceCount; i++)
		{
			if (i >= polyphony || resetAll)
			{
				voices[i].SetGate(0.0);
				voices[i].TurnOff();
			}
		}

		effectiveUnison = (unison > polyphony) ? polyphony : unison;

		// if mono, then effective poly is the number of unison voice
		// otherwise, the round robin algo takes care of allocating voices using specified polyphony
		effectivePolyphony = voiceMode == VoiceMode::PolyRoundRobin
			? polyphony
			: effectiveUnison;

		effectiveVoiceMode = voiceMode;
	}

	void VoiceAllocator::NoteOn(uint8_t note, float velocity)
	{
		noteCounters[note] = noteCounter++;
		int newNote = voiceMode == VoiceMode::PolyRoundRobin ? note : FindNextMonoNote();

		for (int i = 0; i < polyphony; i++)
		{
			if (voices[i].Note == newNote && voices[i].Gate)
				return; // already playing
		}

		currentVelocity = velocity;

		for (int i = 0; i < effectiveUnison; i++)
		{
			int nextVoiceIndex = FindNextVoice();
			voiceCounters[nextVoiceIndex] = voiceCounter++;

			float unisonMin = effectiveUnison == 1 ? 0.0f : -1.0f;
			float unisonMap = effectiveUnison == 1 ? 0.0f : 2.0f * i / (float)(effectiveUnison - 1);
			float unisonValue = unisonMin + unisonMap;

			voices[nextVoiceIndex].SetUnisonValue(unisonValue);
			voices[nextVoiceIndex].SetNote(note);
			voices[nextVoiceIndex].SetGate(velocity);
		}
	}

	void VoiceAllocator::NoteOff(uint8_t note)
	{
		noteCounters[note] = 0;
		int nextNote = voiceMode == VoiceMode::PolyRoundRobin ? -1 : FindNextMonoNote();

		if (nextNote != -1)
		{
			NoteOn(nextNote, currentVelocity);
		}
		if (nextNote == -1)
		{
			for (int i = 0; i < polyphony; i++)
			{
				// there can be multiple voices for the same note, when using unison
				if (voices[i].Note == note && voices[i].Gate)
				{
					voices[i].SetGate(0);
					voiceCounters[i] = voiceCounter++;
				}
			}
		}
	}

	int VoiceAllocator::FindNextMonoNote()
	{
		if (voiceMode == VoiceMode::MonoHighest)
		{
			for (int i = 127; i >= 0; i--)
			{
				if (noteCounters[i] != 0)
					return i;
			}

			return -1;
		}

		if (voiceMode == VoiceMode::MonoLowest)
		{
			for (int i = 0; i < 128; i++)
			{
				if (noteCounters[i] != 0)
					return i;
			}

			return -1;
		}

		if (voiceMode == VoiceMode::MonoNewest)
		{
			int maxCounter = 0;
			int note = -1;
			for (int i = 0; i < 128; i++)
			{
				int counter = noteCounters[i];
				if (counter > maxCounter)
				{
					maxCounter = counter;
					note = i;
				}
			}

			return note;
		}

		return -1;
	}

	int VoiceAllocator::FindNextVoice()
	{
		int minCounter = INT32_MAX;
		int voice = -1;

		// reset all completely Off voices to zero counter
		for (int i = 0; i < effectivePolyphony; i++)
		{
			if (voices[i].GetState() == 0)
				voiceCounters[i] = 0;
		}

		// first, check only released and off voices
		for (int i = 0; i < effectivePolyphony; i++)
		{
			int idx = (lastAllocatedVoice + i + 1) % effectivePolyphony;

			if (voices[idx].GetState() < 2)
			{
				if (voiceCounters[idx] < minCounter)
				{
					minCounter = voiceCounters[idx];
					voice = idx;
				}
			}
		}

		if (voice == -1)
		{
			// if none found, we need to steal an active voice
			for (int i = 0; i < effectivePolyphony; i++)
			{
				int idx = (lastAllocatedVoice + i + 1) % effectivePolyphony;

				if (voiceCounters[idx] < minCounter)
				{
					minCounter = voiceCounters[idx];
					voice = idx;
				}
			}
		}

		lastAllocatedVoice = voice;
		return voice;
	}
}
