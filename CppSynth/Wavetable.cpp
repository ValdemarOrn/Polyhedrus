#include "Wavetable.h"
#include "Wavetables/Pwm.h"
#include "Fft/FastFFT.h"
#include "Fft/Complex.h"

std::vector<Wavetable*> Wavetable::Wavetables;

// These arrays map each midi note to a number of partials and the table index for that partial wave
int Wavetable::WavetablePartials[NumPartials] = { 512,389,291,218,163,122,91,68,51,38,28,21,16,12,9,6,5,3,2,1 };
int Wavetable::WavetableIndex[128] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,2,2,2,2,2,3,3,3,3,3,4,4,4,4,4,5,5,5,5,5,6,6,6,6,6,7,7,7,7,7,8,8,8,8,8,9,9,9,9,9,10,10,10,10,10,11,11,11,11,11,12,12,12,12,12,13,13,13,13,13,14,14,14,14,14,15,15,15,15,15,16,16,16,16,16,17,17,17,17,17,18,18,18,18,18,18,18,18,18,18,18,19,19,19 };

Wavetable* ConvertTable(float* wavetable, int sampleCount, int numTables)
{
	auto output = new Wavetable();
	output->Count = numTables;
	output->SampleSize = sampleCount;
	output->Partials = Wavetable::NumPartials;

	output->WavetableData = new float[Wavetable::NumPartials * numTables * sampleCount];
	FastFFT<double> transform;

	auto LimitPartials = [&](int max, Complex<double>* data) -> void
	{
		for (int i = 0; i < sampleCount; i++)
		{
			if (i > max && (sampleCount - i) > max)
			{
				data[i].Real = 0;
				data[i].Imag = 0;
			}
		}
	};

	for (int partialIndex = 0; partialIndex < Wavetable::NumPartials; partialIndex++)
	{
		auto partial = Wavetable::WavetablePartials[partialIndex];
		auto outputIndexPartial = &(output->WavetableData[partialIndex * (sampleCount * numTables)]);

		for (int tableIndex = 0; tableIndex < numTables; tableIndex++)
		{
			auto outputIndexWave = &(outputIndexPartial[tableIndex * sampleCount]);
			auto table = &wavetable[tableIndex * sampleCount];
			auto input = new Complex<double>[sampleCount];
			auto scratch = new Complex<double>[sampleCount];
			auto fft = new Complex<double>[sampleCount];
			auto ifft = new Complex<double>[sampleCount];

			for (size_t i = 0; i < sampleCount; i++)
				input[i].Real = table[i];
			
			transform.FFT(input, fft, scratch, sampleCount);
			LimitPartials(partial, fft);
			transform.IFFT(fft, ifft, scratch, sampleCount);
			
			for (size_t i = 0; i < sampleCount; i++)
				outputIndexWave[i] = ifft[i].Real;
			
			delete input;
			delete scratch;
			delete fft;
			delete ifft;
		}
	}

	return output;
}

void Normalize(Wavetable* wavetable)
{
	float max = 0.0;
	for (size_t i = 0; i < wavetable->Partials * wavetable->Count * wavetable->SampleSize; i++)
	{
		auto val = wavetable->WavetableData[i];
		if (abs(val) > max)
			max = abs(val);
	}

	auto scale = 1.0 / max;

	for (size_t i = 0; i < wavetable->Partials * wavetable->Count * wavetable->SampleSize; i++)
	{
		wavetable->WavetableData[i] = wavetable->WavetableData[i] * scale;
	}
}

void Wavetable::Setup()
{
	auto sampleCount = 2048;
	auto numTables = 16;
	
	auto pwm = Pwm::CreateTable(sampleCount, numTables);
	auto wavetable = ConvertTable(pwm, sampleCount, numTables);
	Normalize(wavetable);
	Wavetable::Wavetables.push_back(wavetable);
}
