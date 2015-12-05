#include "../Osc/OscMessage.h"
#include <vector>
#include <cassert>
using namespace std;

inline void Reverse(uint8_t* data)
{
	auto a = data[0];
	auto b = data[1];
	auto c = data[2];
	auto d = data[3];

	data[0] = d;
	data[1] = c;
	data[2] = b;
	data[3] = a;
}

using namespace Polyhedrus;

namespace Tests
{
	namespace Osc
	{
		void TestOscParser1()
		{
			char message[32];
			memcpy(message, "/Path\0\0\0,i\0\0\0\0\0A", 16);
			vector<uint8_t> vec(&message[0], &message[16]);

			OscMessage osc(vec);
			assert(osc.Address == "/Path");
			char tag = osc.TypeTags[0];
			assert(strncmp(&tag, "i", 1) == 0);
			assert(osc.TypeTags.size() == 1);

			int val = osc.GetInt(0);
			assert(val == 65);
		}

		void TestOscParserFloatInt()
		{
			uint8_t message[32];
			memcpy(message, "/Path\0\0\0,fi\0", 12);
			float* fp = (float*)&(message[12]);
			int* ip = (int*)&(message[16]);
			*fp = 23.45f;
			*ip = 56;
			Reverse(&message[12]);
			Reverse(&message[16]);
			vector<uint8_t> vec(&message[0], &message[20]);

			OscMessage osc(vec);
			assert(osc.Address == "/Path");
			assert(osc.TypeTags[0] == 'f');
			assert(osc.TypeTags[1] == 'i');
			assert(osc.TypeTags.size() == 2);

			assert(osc.GetFloat(0) == 23.45f);
			assert(osc.GetInt(1) == 56);
		}

		void TestOscParserStringBlobInt()
		{
			uint8_t message[40];
			memcpy(message, "/Path\0\0\0,sbi\0\0\0\0", 16);
			char* sp = (char*)&(message[16]);
			uint8_t* bp = (uint8_t*)&(message[24]);
			int* ip = (int*)&(message[32]);

			sp[0] = 'a';
			sp[1] = 'b';
			sp[2] = 'c';
			sp[3] = 'd';
			sp[4] = 'e';
			sp[5] = 0;
			sp[6] = 0;
			sp[7] = 0;

			bp[0] = 0;
			bp[1] = 0;
			bp[2] = 0;
			bp[3] = 3;
			bp[4] = 24;
			bp[5] = 45;
			bp[6] = 99;
			bp[7] = 0;

			*ip = 42;
			Reverse(&message[32]);

			vector<uint8_t> vec(&message[0], &message[36]);

			OscMessage osc(vec);
			assert(osc.Address == "/Path");
			assert(osc.TypeTags[0] == 's');
			assert(osc.TypeTags[1] == 'b');
			assert(osc.TypeTags[2] == 'i');
			assert(osc.TypeTags.size() == 3);

			assert(osc.GetString(0) == "abcde");
			assert(osc.GetInt(2) == 42);

			auto data = osc.GetBlob(1);
			assert(data.size() == 3);
			assert(data[0] == 24);
			assert(data[1] == 45);
			assert(data[2] == 99);
		}

		void TestBundleParse()
		{
			char message[40];
			memcpy(message, "#bundle\0\0\0\0\0\0\0\0\0\0\0\0\x10/Path\0\0\0,i\0\0\0\0\0A", 36);
			vector<uint8_t> vec(&message[0], &message[36]);

			auto messages = OscMessage::ParseRawBytes(vec);
			auto osc = messages[0];
			assert(osc.Address == "/Path");
			char tag = osc.TypeTags[0];
			assert(strncmp(&tag, "i", 1) == 0);
			assert(osc.TypeTags.size() == 1);

			int val = osc.GetInt(0);
			assert(val == 65);
		}
	}
}
