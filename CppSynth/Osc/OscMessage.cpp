#include "OscMessage.h"

inline int ReadInt(uint8_t* dataPtr)
{
	uint8_t bytes[4];
	bytes[0] = dataPtr[3];
	bytes[1] = dataPtr[2];
	bytes[2] = dataPtr[1];
	bytes[3] = dataPtr[0];
	int* ptr = (int*)bytes;
	int value = *ptr;
	return value;
}

inline float ReadFloat(uint8_t* dataPtr)
{
	uint8_t bytes[4];
	bytes[0] = dataPtr[3];
	bytes[1] = dataPtr[2];
	bytes[2] = dataPtr[1];
	bytes[3] = dataPtr[0];
	float* ptr = (float*)bytes;
	float value = *ptr;
	return value;
}

vector<OscMessage> OscMessage::ParseRawBytes(vector<uint8_t> byteData)
{
	char* ptr = (char*)&byteData[0];
	string header = string(ptr);

	if (header == "#bundle")
	{
		return ParseBundle(byteData);
	}
	else
	{
		vector<OscMessage> output;
		OscMessage msg(byteData);
		output.push_back(msg);
		return output;
	}
}

vector<OscMessage> OscMessage::ParseBundle(vector<uint8_t> dataBundle)
{
	vector<OscMessage> output;
	char* ptr = (char*)&dataBundle[0];
	string header = string(ptr);

	if (header != "#bundle")
		return output;

	int parseIndex = 8;
	parseIndex += 8; // skip timetag

	while (parseIndex < dataBundle.size())
	{
		int size = ReadInt(&dataBundle[parseIndex]);
		parseIndex += 4;
		auto messageData = vector<uint8_t>(ptr + parseIndex, ptr + parseIndex + size);
		auto message = OscMessage(messageData);
		output.push_back(message);

		parseIndex += size;
	}

	return output;
}

OscMessage::OscMessage(std::vector<uint8_t> data)
{
	Data = data;
	int parseIndex = 0;
	uint8_t* dataPtr = &(data[0]);
	parseIndex = ParseAddress(dataPtr, parseIndex);
	
	if (parseIndex == data.size())
		return; // empty message

	parseIndex = ParseTypeTags(dataPtr, parseIndex);

	for (auto tag : TypeTags)
	{
		ArgumentIndices.push_back(parseIndex);

		if (tag == 'i')
			parseIndex = ParseInt(dataPtr, parseIndex);
		else if (tag == 'f')
			parseIndex = ParseFloat(dataPtr, parseIndex);
		else if (tag == 's')
			parseIndex = ParseString(dataPtr, parseIndex);
		else if (tag == 'b')
			parseIndex = ParseBlob(dataPtr, parseIndex);
	}
}

int OscMessage::GetInt(int argumentIndex)
{
	auto index = ArgumentIndices[argumentIndex];
	int value = ReadInt(&Data[index]);
	return value;
}

float OscMessage::GetFloat(int argumentIndex)
{
	auto index = ArgumentIndices[argumentIndex];
	float value = ReadFloat(&Data[index]);
	return value;
}

string OscMessage::GetString(int argumentIndex)
{
	auto index = ArgumentIndices[argumentIndex];
	auto ptr = &Data[index];
	char* floatPtr = (char*)ptr;
	string str(floatPtr);
	return str;
}

vector<uint8_t> OscMessage::GetBlob(int argumentIndex)
{
	auto index = ArgumentIndices[argumentIndex];
	int size = ReadInt(&Data[index]);

	auto ptr = &Data[index] + 4;
	vector<uint8_t> data(ptr, ptr + size);
	return data;
}

int OscMessage::ParseAddress(uint8_t* data, int parseIndex)
{
	bool foundNull = false;

	while (true)
	{
		if (data[parseIndex] == 0)
			foundNull = true;

		if (foundNull && parseIndex % 4 == 3)
		{
			parseIndex++;
			char* addr = (char*)&(data[0]);
			Address = string(addr);
			return parseIndex;
		}
		else
			parseIndex++;
	}
}

int OscMessage::ParseTypeTags(uint8_t* data, int parseIndex)
{
	if (data[parseIndex] != ',')
		return -1;

	parseIndex++;

	while (true)
	{
		if (data[parseIndex] == 0)
		{
			return ParseNulls(data, parseIndex);
		}
		else
		{
			TypeTags.push_back(data[parseIndex]);
			parseIndex++;
		}
	}
}

int OscMessage::ParseInt(uint8_t* data, int parseIndex)
{
	return parseIndex + 4;
}

int OscMessage::ParseFloat(uint8_t* data, int parseIndex)
{
	return parseIndex + 4;
}

int OscMessage::ParseString(uint8_t* data, int parseIndex)
{
	return ParseNulls(data, parseIndex + 1);
}

int OscMessage::ParseBlob(uint8_t* data, int parseIndex)
{
	int size = ReadInt(&data[parseIndex]);
	parseIndex += 4;
	parseIndex += size;
	return ParseNulls(data, parseIndex);
}

int OscMessage::ParseNulls(uint8_t * data, int parseIndex)
{
	while (true)
	{
		if (parseIndex % 4 == 0 && data[parseIndex - 1] == 0)
			return parseIndex;

		parseIndex++;
	}
}
