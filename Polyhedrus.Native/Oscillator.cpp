#include "Oscillator.h"
#include "AudioLib/Utils.h"
#include <iostream>

namespace Polyhedrus
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
		this->buffer = new float[bufferSize]();
		this->FmBuffer = new float[bufferSize]();
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
		float divisor = (float)std::pow(2, -7);
		float octavesPerSecond = (float)std::pow(2, -8 * value) / divisor;
		float notesPerSample = (float)(octavesPerSecond * 12.0 / samplerate);
		glideRate = notesPerSample;
	}

	void Oscillator::Reset()
	{
		if (Phase < 0.999) // Phase = 1.0 := Free phase
			iterator = (int)(UINT32_MAX * Phase);
	}

	void Oscillator::Process(int count)
	{
		Update();

		for (int i = 0; i < count; i++)
		{
			uint32_t iterWithFm = (iterator + (uint32_t)(FmBuffer[i] * UINT32_MAX));
			//float fIndex = iterWithFm / (double)UINT32_MAX * tableSize;
			float fIndex = iterWithFm * iteratorScaler;

			int idx1 = (int)fIndex;
			int idx2 = idx1 + 1;
			if (idx2 >= tableSize) // ripe for optimization!!
				idx2 = 0;
			float mixer = fIndex - idx1;

			float wa = waveA[idx1] * (1 - mixer) + waveA[idx2] * mixer;
			float wb = waveB[idx1] * (1 - mixer) + waveB[idx2] * mixer;

			buffer[i] = wa * (1 - waveMix) + wb * waveMix;
			iterator += increment;
		}
	}

	float* Oscillator::GetOutput()
	{
		return buffer;
	}

	std::vector<uint8_t> Oscillator::GetVisual()
	{
		std::vector<uint8_t> output;
		std::vector<float> floatOut;
		std::shared_ptr<Wavetable> wt = wavetable;

		float waveIndex = (Shape + ShapeMod) * wavetable->Count;
		bool useNextWave = (waveIndex < wavetable->Count - 1);
		float mix = waveIndex - (int)waveIndex;
		int index1 = (int)waveIndex;
		int index2 = index1 + useNextWave;

		// I choose partial index 5 as it has 1024 samples in it. no need to show the biggest table, we just get image aliasing in the Gui
		auto table1 = wt->GetTable(index1, 5);
		auto table2 = wt->GetTable(index2, 5);
		int size = wt->WavetableSize[5];

		float max = -1;
		float min = 1;

		for (int i = 0; i < size; i++)
		{
			float val = table1[i] * (1 - mix) + table2[i] * mix;
			floatOut.push_back(val);
			if (val > max)
				max = val;
			if (val < min)
				min = val;
		}

		if (max - min < 0.1f)
			max = min + 0.1f;

		float scale = 1.0f / (max - min);

		for (int i = 0; i < size; i++)
		{
			float val = (floatOut[i] - min) * scale;
			uint8_t val2 = (uint8_t)(val * 255.99);
			output.push_back(val2);
		}

		return output;
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
		pitch = AudioLib::Utils::Limit(pitch, 0.0f, 127.999f);
		int partialIndex = wavetable->WavetableIndex[(int)pitch];
		waveMix = waveIndex - (int)waveIndex;

		bool useNextWave = (waveIndex < wavetable->Count - 1);
		waveA = wavetable->GetTable((int)waveIndex, partialIndex);
		waveB = wavetable->GetTable((int)waveIndex + useNextWave, partialIndex);

		tableSize = wavetable->WavetableSize[partialIndex];
		iteratorScaler = (float)((1.0 / (double)UINT32_MAX) * tableSize);

		float freq = AudioLib::Utils::Note2Freq(pitch) + (Linear + LinearMod);
		float samplesPerCycle = samplerate / freq;
		increment = (uint32_t)((1.0f / samplesPerCycle) * UINT32_MAX);
	}
}
