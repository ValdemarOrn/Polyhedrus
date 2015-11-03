#include "WavetableManager.h"
#include "Wavetables/Pwm.h"
#include "Wavetables/Sawtooth.h"
#include "Fft/FastFFT.h"
#include "Fft/Complex.h"
#include "boost/filesystem.hpp"
#include "AudioLib/WaveFile.h"

using std::vector;
using std::string;
using namespace boost::filesystem;

namespace Leiftur
{
	std::vector<std::string> WavetableManager::WavetableFiles;
	//std::vector<Wavetable*> WavetableManager::Wavetables;
	int WavetableManager::TotalSize;

	// These arrays map each midi note to a number of partials and the table index for that partial wave
	int WavetableManager::WavetablePartials[NumPartials] = { 512,389,291,218,163,122,91,68,51,38,28,21,16,12,9,6,5,3,2,1 };
	int WavetableManager::WavetableSize[NumPartials] = { 2048,2048,1024,1024,1024,512,512,512,256,256,128,128,128,128,128,128,128,128,128,128 };
	int WavetableManager::WavetableOffset[NumPartials];
	int WavetableManager::WavetableIndex[128] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,2,2,2,2,2,3,3,3,3,3,4,4,4,4,4,5,5,5,5,5,6,6,6,6,6,7,7,7,7,7,8,8,8,8,8,9,9,9,9,9,10,10,10,10,10,11,11,11,11,11,12,12,12,12,12,13,13,13,13,13,14,14,14,14,14,15,15,15,15,15,16,16,16,16,16,17,17,17,17,17,18,18,18,18,18,18,18,18,18,18,18,19,19,19 };

	std::shared_ptr<Wavetable> ConvertTable(float* wavetable, int tableSize, int numTables)
	{
		if (tableSize != 2048)
			return 0;  //  Limit to 2048 only for simplification, Todo: Improve

		std::shared_ptr<Wavetable> output(new Wavetable(WavetableManager::TotalSize * numTables));
		output->Count = numTables;
		//output->WavetableData = new float[WavetableManager::TotalSize * numTables];
		output->WavetableDataSize = WavetableManager::TotalSize * numTables;
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

			for (int partialIndex = 0; partialIndex < WavetableManager::NumPartials; partialIndex++)
			{
				auto partialCount = WavetableManager::WavetablePartials[partialIndex];
				auto tableSize = WavetableManager::WavetableSize[partialIndex];
				auto tablePtr = output->GetTable(tableIndex, partialIndex);

				auto fftSubset = LimitPartials(tableSize, partialCount);
				transform.IFFT(&fftSubset[0], ifft, scratch, tableSize);

				for (size_t i = 0; i < tableSize; i++)
					tablePtr[i] = ifft[i].Real;
			}
		}

		return output;
	}

	void Normalize(std::shared_ptr<Wavetable> wavetable)
	{
		float max = 0.0;
		float* wt = wavetable->GetTable(0, 0);
		for (int i = 0; i < wavetable->Count * WavetableManager::TotalSize; i++)
		{
			if (abs(wt[i]) > max)
				max = abs(wt[i]);
		}

		auto scale = 1.0 / max;

		for (int i = 0; i < wavetable->Count * WavetableManager::TotalSize; i++)
		{
			wt[i] = wt[i] * scale;
		}
	}

	vector<string> ScanWavetableFiles(std::string pluginDirectory)
	{
		auto waveformDirectory = path(pluginDirectory) / "Waveforms";
		auto wavetablePath = waveformDirectory / "Wavetables";
		int dirLen = waveformDirectory.string().length();
		directory_iterator end_itr;
		vector<string> subdirs;
		vector<string> files;

		for (directory_iterator itr(wavetablePath); itr != end_itr; ++itr)
		{
			if (is_directory(itr->status()))
			{
				auto dir = itr->path().filename().string();
				subdirs.push_back(dir);
			}
		}

		for (auto subdir : subdirs)
		{
			auto subPath = wavetablePath / subdir;

			for (directory_iterator itr(subPath); itr != end_itr; ++itr)
			{
				if (itr->path().extension() == ".wav")
				{
					auto fullPath = itr->path();
					files.push_back(fullPath.string().substr(0)); // Todo: chop the path correctly
				}
			}
		}

		return files;
	}

	void WavetableManager::Setup(std::string waveformDirectory)
	{
		WavetableFiles = ScanWavetableFiles(waveformDirectory);

		auto sampleCount = 2048;
		auto numTables = 16;

		int sum = 0;
		for (int i = 0; i < NumPartials; i++)
		{
			WavetableOffset[i] = sum;
			sum += WavetableSize[i];
		}

		TotalSize = sum;

		/*auto saw = Wavetables::Sawtooth::CreateTable(sampleCount, numTables);
		auto wavetable = ConvertTable(saw, sampleCount, numTables);
		Normalize(wavetable);
		WavetableManager::Wavetables.push_back(wavetable);

		auto pwm = Wavetables::Pwm::CreateTable(sampleCount, numTables);
		wavetable = ConvertTable(pwm, sampleCount, numTables);
		Normalize(wavetable);
		WavetableManager::Wavetables.push_back(wavetable);*/
	}

	std::shared_ptr<Wavetable> WavetableManager::LoadWavetable(int wtNum)
	{
		auto file = WavetableFiles.at(wtNum);
		auto dat = AudioLib::WaveFile::ReadWaveFile(file);
		auto data = dat.at(0);
		auto wavetable = ConvertTable(&data[0], 2048, 256);
		Normalize(wavetable);
		return wavetable;
	}
}