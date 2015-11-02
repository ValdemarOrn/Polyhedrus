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
		wavetable = WavetableManager::Wavetables[table];
		Update();
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

		float pitch = Note + 12.0f * Octave + Semi + 0.01f * Cent + 24.0f * PitchMod;
		pitch = AudioLib::Utils::Limit(pitch, 0.0, 127.9999);
		int partialIndex = WavetableManager::WavetableIndex[(int)pitch];
		waveMix = waveIndex - (int)waveIndex;

		bool useNextWave = (waveIndex < wavetable->Count - 1);
		waveA = wavetable->GetTable((int)waveIndex, partialIndex);
		waveB = wavetable->GetTable((int)waveIndex + useNextWave, partialIndex);

		shiftValue = (32 - (int)(log2(WavetableManager::WavetableSize[partialIndex]) + 0.01)); // how many bits of the iterator are used to address the table

		float freq = AudioLib::Utils::Note2Freq(pitch);
		float samplesPerCycle = samplerate / freq;
		increment = (uint32_t)((1.0f / samplesPerCycle) * UINT32_MAX);
	}
}
