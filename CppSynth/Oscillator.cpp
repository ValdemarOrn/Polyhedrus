#include "Oscillator.h"
#include "AudioLib/Utils.h"
#include <iostream>

namespace Leiftur
{
	Oscillator::Oscillator()
	{
		buffer = 0;
		FmBuffer = 0;
		Keytrack = true;
		Linear = 0;
		LinearMod = 0;
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
	}

	Oscillator::~Oscillator()
	{
		delete buffer;
		delete FmBuffer;
	}

	void Oscillator::Initialize(int samplerate, int bufferSize, int modulationUpdateRate)
	{
		this->buffer = new float[bufferSize];
		this->FmBuffer = new float[bufferSize];
		this->modulationUpdateRate = modulationUpdateRate;
		this->samplerate = samplerate;
	}

	void Oscillator::SetWavetable(std::shared_ptr<Wavetable> wavetable)
	{
		this->wavetable = wavetable;
		Update();
	}

	void Oscillator::SetGlide(float value)
	{
		if (value < 0.0001)
		{
			glideRate = 10000;
			return;
		}

		// formula: 2 ^ (-8*x) / (2^(-7))
		// goes from 32 oct/second to 0.5 oct/second
		float divisor = std::pow(2, -7);
		float octavesPerSecond = std::pow(2, -8 * value) / divisor;
		float notesPerSample = octavesPerSecond * 12.0 / samplerate;
		glideRate = notesPerSample;
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

			uint32_t iterWithFm = (iterator + (uint32_t)(FmBuffer[i] * UINT32_MAX)) >> shiftValue;

			buffer[i] = waveA[iterWithFm] * (1 - waveMix) + waveB[iterWithFm] * waveMix;
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
		float basePitch = (Keytrack ? Note : 60) + 12.0f * Octave + Semi + 0.01f * Cent;

		if (currentPitch < basePitch)
		{
			currentPitch += glideRate * modulationUpdateRate;
			if (currentPitch > basePitch)
				currentPitch = basePitch;
		}
		else if (currentPitch > basePitch)
		{
			currentPitch -= glideRate * modulationUpdateRate;
			if (currentPitch < basePitch)
				currentPitch = basePitch;
		}

		float pitch = currentPitch + 24.0f * PitchMod;
		pitch = AudioLib::Utils::Limit(pitch, 0.0, 127.9999);
		int partialIndex = wavetable->WavetableIndex[(int)pitch];
		waveMix = waveIndex - (int)waveIndex;

		bool useNextWave = (waveIndex < wavetable->Count - 1);
		waveA = wavetable->GetTable((int)waveIndex, partialIndex);
		waveB = wavetable->GetTable((int)waveIndex + useNextWave, partialIndex);

		shiftValue = (32 - (int)(log2(wavetable->WavetableSize[partialIndex]) + 0.01)); // how many bits of the iterator are used to address the table

		float freq = AudioLib::Utils::Note2Freq(pitch) + (Linear + LinearMod);
		float samplesPerCycle = samplerate / freq;
		increment = (uint32_t)((1.0f / samplesPerCycle) * UINT32_MAX);
	}
}
