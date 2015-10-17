#include "Oscillator.h"
#include "AudioLib/Utils.h"
#include <iostream>

namespace Leiftur
{
	Oscillator::Oscillator()
	{
		buffer = 0;

		Note = 60;
		Octave = 0;
		Semi = 0;
		Cent = 0;
		PitchMod = 0;
		updateCounter = 0;
		iterator = 0;
		Phase = 1.0;
		Shape = 0.0;
		ShapeMod = 0;
		SetWaveform(0);
	}

	Oscillator::~Oscillator()
	{
		delete buffer;
	}

	void Oscillator::Initialize(int samplerate, int bufferSize, int modulationUpdateRate)
	{
		this->buffer = new float[bufferSize];
		this->modulationUpdateRate = modulationUpdateRate;
		this->samplerate = samplerate;
	}

	void Oscillator::SetWaveform(int table)
	{
		wavetable = Wavetable::Wavetables[0];
		shiftValue = (32 - (int)(log2(wavetable->SampleSize) + 0.01)); // how many bits of the iterator are used to address the table
	}

	void Oscillator::Reset()
	{
		if (Phase < 0.999) // Phase = 1.0 := Free phase
			iterator = (int)(UINT32_MAX * Phase);
	}

	void Oscillator::Process(int count)
	{
		for (int i = 0; i < count; i++)
		{
			if (updateCounter == 0)
			{
				Update();
				updateCounter = modulationUpdateRate;
			}

			buffer[i] = waveA[iterator >> shiftValue] * (1 - waveMix) + waveB[iterator >> shiftValue] * waveMix;
			iterator += increment;

			updateCounter--;
		}
	}

	float* Oscillator::GetOutput()
	{
		return buffer;
	}

	void Oscillator::Update()
	{
		float waveIndex = (Shape + ShapeMod) * wavetable->Count;

		auto pitch = Note + 12 * Octave + Semi + 0.01 * Cent + 24 * PitchMod;
		auto partialIndex = Wavetable::WavetableIndex[(int)pitch];
		waveMix = waveIndex - (int)waveIndex;

		bool useNextWave = (waveIndex < wavetable->Count - 1);
		waveA = wavetable->GetTable(partialIndex, waveIndex);
		waveB = wavetable->GetTable(partialIndex, waveIndex + useNextWave);

		float freq = AudioLib::Utils::Note2Freq(pitch);
		float samplesPerCycle = samplerate / freq;
		increment = (1.0 / samplesPerCycle) * UINT32_MAX;
	}
}
