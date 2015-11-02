#ifndef AUDIOLIB_WAVEFILE
#define AUDIOLIB_WAVEFILE

#include <vector>
#include <string>
#include <stdint.h>

using std::string;
using std::vector;

namespace AudioLib
{
	class FormatChunkData
	{
	public:
		int AudioFormat;
		int NumChannels;
		int SampleRate;
		int ByteRate;
		int BlockAlign;
		int BitsPerSample;
		int BytesPerSample;
	};

	class WaveFile
	{
	public:
		/// <summary>
		/// Read a WAVE file. Supports multiple channels, any bitrate.
		/// Supported formats are IEEE 32bit floating point and uncompressed PCM 8/16/24/32 bit
		/// </summary>
		/// <param name="filename"></param>
		/// <returns></returns>
		static vector<vector<double>> ReadWaveFile(string filename);

		/// <summary>
		/// Read a WAVE file. Supports multiple channels, any bitrate.
		/// Supported formats are IEEE 32bit floating point and uncompressed PCM 8/16/24/32 bit
		/// </summary>
		/// <param name="filename"></param>
		/// <returns></returns>
		static vector<vector<double>> ReadWaveFile(const vector<uint8_t>& data);

	private:

		/// <summary>
		/// Parses the data chunk, containing the audio data
		/// </summary>
		/// <param name="data">data array to work on</param>
		/// <param name="idx">starting index of the data chunk</param>
		/// <param name="chunkSize">size of the data chunk</param>
		/// <param name="format">the format description, read from the 'fmt ' chunk</param>
		/// <returns></returns>
		static vector<vector<double>> ParseDataChunk(const vector<uint8_t>& data, int idx, int chunkSize, FormatChunkData format);

		/// <summary>
		/// Reads the fmt chunk
		/// </summary>
		/// <param name="data"></param>
		/// <param name="idx"></param>
		/// <returns></returns>
		static FormatChunkData ParseFmtChunk(const vector<uint8_t>& data, int idx);
	};
}

#endif
