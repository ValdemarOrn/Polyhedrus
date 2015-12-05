#include "WaveFile.h"
#include <fstream>
#include <iterator>

using namespace std;

namespace AudioLib
{
	vector<vector<float>> WaveFile::ReadWaveFile(string filename)
	{
		ifstream file(filename, ios::binary);
		// Stop eating new lines in binary mode
		file.unsetf(ios::skipws);

		// get its size:
		streampos fileSize;
		file.seekg(0, std::ios::end);
		fileSize = file.tellg();
		file.seekg(0, std::ios::beg);

		// reserve capacity
		std::vector<uint8_t> vec;
		vec.reserve(fileSize);

		// read the data:
		vec.insert(vec.begin(),
			std::istream_iterator<uint8_t>(file),
			std::istream_iterator<uint8_t>());

		return ReadWaveFile(vec);
	}

	vector<vector<float>> WaveFile::ReadWaveFile(const vector<uint8_t>& data)
	{
		uint8_t waveFormat[5] = { data[8], data[9], data[10], data[11], 0 };
		string fmt((char*)waveFormat);

		if (fmt != "WAVE")
			return vector<vector<float>>();

		int idx = 12;
		FormatChunkData format;
		vector<vector<float>> output;

		while (idx < data.size())
		{

			uint8_t chunkString[5] = { data[idx + 0], data[idx + 1], data[idx + 2], data[idx + 3], 0 };
			string chunkId((char*)chunkString);
			int chunkSize = *((int*)&data[idx + 4]);

			if (chunkId == "fmt ")
			{
				format = ParseFmtChunk(data, idx);
			}

			if (chunkId == "data")
			{
				if (format.ByteRate == 0) // format must preced data.
					return vector<vector<float>>();

				output = ParseDataChunk(data, idx, chunkSize, format);
				break;
			}

			idx = idx + 8 + chunkSize;
		}

		return output;
	}

	vector<vector<float>> WaveFile::ParseDataChunk(const vector<uint8_t>& data, int idx, int chunkSize, FormatChunkData format)
	{
		vector<vector<float>> channels;

		for (int i = 0; i < format.NumChannels; i++)
			channels.push_back(vector<float>());

		idx += 8;
		int channel = 0;

		for (int i = idx; i < chunkSize + idx; i += format.BytesPerSample)
		{
			uint8_t byteData[4];
			int value = 0;

			if (format.AudioFormat == 3) // audioFormat 3 indicated IEEE 32bit floating point data
			{
				float val = *((float*)&data[i]);
				channels[channel].push_back(val);
			}
			else if (format.BytesPerSample == 1) // 8 bit PCM data
			{
				value = data[i] - 0x80;
				channels[channel].push_back(value / 128.0);
			}
			else if (format.BytesPerSample == 2) // 16 bit PCM data
			{
				value = *((int16_t*)&data[i]);
				channels[channel].push_back(value / 32768.0);
			}
			else if (format.BytesPerSample == 3) // 24 bit PCM data
			{
				byteData[0] = 0;
				byteData[1] = data[i];
				byteData[2] = data[i + 1];
				byteData[3] = data[i + 2];
				value = *((int32_t*)byteData);
				channels[channel].push_back(value / 2147483648.0);
			}
			else if (format.BytesPerSample == 4) // 32 bit PCM data
			{
				value = *((int32_t*)&data[i]);
				channels[channel].push_back(value / 2147483648.0);
			}

			channel = (channel + 1) % format.NumChannels;
		}

		return channels;
	}

	FormatChunkData WaveFile::ParseFmtChunk(const vector<uint8_t>& data, int idx)
	{
		idx = idx + 8;
		FormatChunkData fmt;
		fmt.AudioFormat = data[idx];
		fmt.NumChannels = data[idx + 2];
		fmt.SampleRate = *((int32_t*)&data[idx + 4]);
		fmt.ByteRate = *((int32_t*)&data[idx + 8]);
		fmt.BlockAlign = *((int16_t*)&data[idx + 12]);
		fmt.BitsPerSample = *((int16_t*)&data[idx + 14]);
		fmt.BytesPerSample = fmt.BitsPerSample / 8;

		return fmt;
	}
}
