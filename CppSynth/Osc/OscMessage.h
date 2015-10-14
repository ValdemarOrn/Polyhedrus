#ifndef OSC_MESSAGE
#define OSC_MESSAGE

#include <vector>
#include <string>
#include <stdint.h>

using std::vector;
using std::string;

class OscMessage
{
public:
	string Address;
	vector<char> TypeTags;
	vector<uint8_t> Data;
	vector<int> ArgumentIndices;
public:

	static vector<OscMessage> ParseRawBytes(vector<uint8_t> byteData);
	static vector<OscMessage> ParseBundle(vector<uint8_t> dataBundle);

	OscMessage(vector<uint8_t> data);
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

#endif
