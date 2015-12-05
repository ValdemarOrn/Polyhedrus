#include "WavetableManager.h"
#include "Fft/FastFFT.h"
#include "Fft/Complex.h"
#include "boost/filesystem.hpp"
#include "AudioLib/WaveFile.h"

using std::vector;
using std::string;
using namespace boost::filesystem;

namespace Polyhedrus
{
	std::shared_ptr<Wavetable> WavetableManager::ConvertTable(float* wavetable, int tableSize, int numTables)
	{
		if (tableSize != 2048)
			return 0;  //  Limit to 2048 only for simplification, Todo: Improve

		std::shared_ptr<Wavetable> output(new Wavetable(WavetableManager::TotalSize * numTables));
		output->Count = numTables;
		output->TotalSize = TotalSize;
		output->WavetableDataSize = TotalSize * numTables;

		for (int i = 0; i < 128; i++)
			output->WavetableIndex[i] = WavetableIndex[i];
		
		for (int i = 0; i < NumWavetablePartials; i++)
		{
			output->WavetablePartials[i] = WavetablePartials[i];
			output->WavetableSize[i] = WavetableSize[i];
			output->WavetableOffset[i] = WavetableOffset[i];
		}
				
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

			for (int partialIndex = 0; partialIndex < NumWavetablePartials; partialIndex++)
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

	void WavetableManager::Normalize(std::shared_ptr<Wavetable> wavetable)
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

	std::vector<WavetableFile> WavetableManager::ScanWavetableFiles(std::string pluginDirectory)
	{
		auto waveformDirectory = path(pluginDirectory) / "Waveforms";
		auto wavetablePath = waveformDirectory / "Wavetables";
		int dirLen = waveformDirectory.string().length();
		directory_iterator end_itr;
		vector<string> subdirs;
		vector<WavetableFile> files;

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
					string selector = fullPath.string().substr(dirLen);
					std::replace(selector.begin(), selector.end(), '\\', '/');
					selector = selector.substr(0, selector.length() - 4);

					WavetableFile file;
					file.FilePath = fullPath.string();
					file.Index = files.size();
					file.Selector = selector;
					files.push_back(file);
				}
			}
		}

		WavetableFiles = files;
		loadedWavetables.clear();
		for (int i = 0; i < WavetableFiles.size(); i++)
			loadedWavetables.push_back(std::weak_ptr<Wavetable>());
		
		return files;
	}

	void WavetableManager::Setup(std::string waveformDirectory)
	{
		ScanWavetableFiles(waveformDirectory);
		
		auto sampleCount = 2048;
		auto numTables = 16;

		int sum = 0;
		for (int i = 0; i < NumWavetablePartials; i++)
		{
			WavetableOffset[i] = sum;
			sum += WavetableSize[i];
		}

		TotalSize = sum;
	}

	std::shared_ptr<Wavetable> WavetableManager::LoadWavetable(int wtNum)
	{
		if (wtNum < 0 || wtNum >= WavetableFiles.size())
			wtNum = 0;

		if (auto spt = loadedWavetables[wtNum].lock())
		{
			return spt; // return cached wavetable pointer if it exists
		}
		
		auto file = WavetableFiles.at(wtNum);
		auto dat = AudioLib::WaveFile::ReadWaveFile(file.FilePath);
		auto data = dat.at(0);
		int tableSize = data.size() / 2048;
		auto wavetable = ConvertTable(&data[0], 2048, tableSize);
		Normalize(wavetable);
		loadedWavetables[wtNum] = wavetable;
		return wavetable;
	}

	int WavetableManager::GetId(std::string selector)
	{
		for (int i = 0; i < WavetableFiles.size(); i++)
		{
			if (WavetableFiles.at(i).Selector == selector)
				return i;
		}

		return 0; // fallback to wavetable zero
	}
}