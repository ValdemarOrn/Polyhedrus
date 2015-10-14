#include "Oscillator.h"
#include "AudioLib/Utils.h"
#include <iostream>

namespace Leiftur
{
	Oscillator::Oscillator()
	{
		Note = 60;
		updateCounter = 0;
		iterator = 0;
		SetWavetable(0);
	}

	void Oscillator::Initialize(int samplerate)
	{
		this->samplerate = samplerate;
	}

	void Oscillator::SetWavetable(int table)
	{
		wavetable = Wavetable::Wavetables[0];
		shiftValue = (32 - (int)(log2(wavetable->SampleSize) + 0.01)); // how many bits of the iterator are used to address the table
	}

	void Oscillator::Update()
	{
		auto pitch = Note + PitchBend * 2;
		auto partialIndex = Wavetable::WavetableIndex[(int)pitch];
		waveMix = WaveIndex - (int)WaveIndex;
		waveA = wavetable->GetTable(partialIndex, WaveIndex);
		waveB = wavetable->GetTable(partialIndex, WaveIndex + (WaveIndex < wavetable->Count - 1));

		float freq = AudioLib::Utils::Note2Freq(pitch);
		float samplesPerCycle = samplerate / freq;
		increment = (1.0 / samplesPerCycle) * UINT32_MAX;
	}

	void Oscillator::GetSamples(float * buffer, int count)
	{
		for (int i = 0; i < count; i++)
		{
			if (updateCounter == 0)
			{
				Update();
				updateCounter = 8;
			}

			buffer[i] = waveA[iterator >> shiftValue] * (1 - waveMix) + waveB[iterator >> shiftValue] * waveMix;
			iterator += increment;

			updateCounter--;
		}
	}
}
