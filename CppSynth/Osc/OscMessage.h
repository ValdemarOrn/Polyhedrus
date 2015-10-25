#ifndef LEIFTUR_OSC_MESSAGE
#define LEIFTUR_OSC_MESSAGE

#include <vector>
#include <string>
#include <stdint.h>

using std::vector;
using std::string;

namespace Leiftur
{
	class OscMessage
	{
	public:
		string Address;
		vector<char> TypeTags;
		vector<uint8_t> Data;
		vector<int> ArgumentIndices;
	private:
		bool isWriteable;
	public:

		static vector<OscMessage> ParseRawBytes(vector<uint8_t> byteData);
		static vector<OscMessage> ParseBundle(vector<uint8_t> dataBundle);

		OscMessage(vector<uint8_t> data);
		OscMessage(std::string address);

		void SetInt(int value);
		void SetFloat(float value);
		void SetString(std::string value);
		void SetBlob(vector<uint8_t> data);
		vector<uint8_t> GetBytes();

		int GetInt(int argumentIndex);
		float GetFloat(int argumentIndex);
		string GetString(int argumentIndex);
		vector<uint8_t> GetBlob(int argumentIndex);

	private:
		int ParseAddress(uint8_t* data, int parseIndex);
		int ParseTypeTags(uint8_t* data, int parseIndex);

		int ParseInt(uint8_t* data, int parseIndex);
		int ParseFloat(uint8_t* data, int parseIndex);
		int ParseString(uint8_t* data, int parseIndex);
		int ParseBlob(uint8_t* data, int parseIndex);

		int ParseNulls(uint8_t* data, int parseIndex);
	};
}
#endif
