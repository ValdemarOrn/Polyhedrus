#include "Wavetable.h"
#include "Wavetables/Pwm.h"
#include "Wavetables/Sawtooth.h"
#include "Fft/FastFFT.h"
#include "Fft/Complex.h"

using std::vector;

namespace Leiftur
{
	std::vector<Wavetable*> Wavetable::Wavetables;
	int Wavetable::TotalSize;

	// These arrays map each midi note to a number of partials and the table index for that partial wave
	int Wavetable::WavetablePartials[NumPartials] = { 512,389,291,218,163,122,91,68,51,38,28,21,16,12,9,6,5,3,2,1 };
	int Wavetable::WavetableSize[NumPartials] = { 2048,2048,1024,1024,1024,512,512,512,256,256,128,128,128,128,128,128,128,128,128,128 };
	int Wavetable::WavetableOffset[NumPartials];
	int Wavetable::WavetableIndex[128] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,2,2,2,2,2,3,3,3,3,3,4,4,4,4,4,5,5,5,5,5,6,6,6,6,6,7,7,7,7,7,8,8,8,8,8,9,9,9,9,9,10,10,10,10,10,11,11,11,11,11,12,12,12,12,12,13,13,13,13,13,14,14,14,14,14,15,15,15,15,15,16,16,16,16,16,17,17,17,17,17,18,18,18,18,18,18,18,18,18,18,18,19,19,19 };

	Wavetable* ConvertTable(float* wavetable, int tableSize, int numTables)
	{
		if (tableSize != 2048)
			return 0;  //  Limit to 2048 only for simplification, Todo: Improve

		auto output = new Wavetable();
		output->Count = numTables;
		output->WavetableData = new float[Wavetable::TotalSize * numTables];
		output->WavetableDataSize = Wavetable::TotalSize * numTables;
		FastFFT<double> transform;

		const int fftSize = 2048;
		Complex<double> input[fftSize];
		Complex<double> scratch[fftSize];
		Complex<double> fft[fftSize];
		Complex<double> ifft[fftSize];

		auto LimitPartials = [&](int size, int maxPartials) -> vector<Complex<double>>
		{
			vector<Complex<double>> vec(size);

			for (int i = 0; i < size / 2; i++)
			{
				if (i == 0)
				{
					vec[0].Real = fft[0].Real;
					vec[0].Imag = fft[0].Imag;
				}
				else if (i <= maxPartials)
				{
					vec[i].Real = fft[i].Real;
					vec[i].Imag = fft[i].Imag;
					vec[size - i].Real = fft[fftSize - i].Real;
					vec[size - i].Imag = fft[fftSize - i].Imag;
				}
				else
				{
					vec[i].Real = 0;
					vec[i].Imag = 0;
					vec[size - i].Real = 0;
					vec[size - i].Imag = 0;
				}
			}

			// scale the samples to compensate for the reduced fft size
			double scaler = size / (double)fftSize;
			for (int i = 0; i < size; i++)
			{
				vec[i].Real *= scaler;
				vec[i].Imag *= scaler;
			}

			return vec;
		};

		for (int tableIndex = 0; tableIndex < numTables; tableIndex++)
		{
			auto table = &wavetable[tableIndex * tableSize];
			
			for (size_t i = 0; i < tableSize; i++)
					input[i].Real = table[i];
			
			transform.FFT(input, fft, scratch, tableSize);

			for (int partialIndex = 0; partialIndex < Wavetable::NumPartials; partialIndex++)
			{
				auto partialCount = Wavetable::WavetablePartials[partialIndex];
				auto tableSize = Wavetable::WavetableSize[partialIndex];
				auto tablePtr = output->GetTable(tableIndex, partialIndex);

				auto fftSubset = LimitPartials(tableSize, partialCount);
				transform.IFFT(&fftSubset[0], ifft, scratch, tableSize);

				for (size_t i = 0; i < tableSize; i++)
					tablePtr[i] = ifft[i].Real;
			}
		}

		return output;
	}

	void Normalize(Wavetable* wavetable)
	{
		float max = 0.0;
		float* wt = wavetable->GetTable(0, 0);
		for (int i = 0; i < wavetable->Count * Wavetable::TotalSize; i++)
		{
			if (abs(wt[i]) > max)
				max = abs(wt[i]);
		}

		auto scale = 1.0 / max;

		for (int i = 0; i < wavetable->Count * Wavetable::TotalSize; i++)
		{
			wt[i] = wavetable->WavetableData[i] * scale;
		}
	}

	void Wavetable::Setup()
	{
		auto sampleCount = 2048;
		auto numTables = 16;

		int sum = 0;
		for (int i = 0; i < NumPartials; i++)
		{
			WavetableOffset[i] = sum;
			sum += WavetableSize[i];
		}

		TotalSize = sum;

		auto saw = Wavetables::Sawtooth::CreateTable(sampleCount, numTables);
		auto wavetable = ConvertTable(saw, sampleCount, numTables);
		Normalize(wavetable);
		Wavetable::Wavetables.push_back(wavetable);

		auto pwm = Wavetables::Pwm::CreateTable(sampleCount, numTables);
		wavetable = ConvertTable(pwm, sampleCount, numTables);
		Normalize(wavetable);
		Wavetable::Wavetables.push_back(wavetable);
	}
}
