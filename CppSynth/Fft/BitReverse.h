
#ifndef BITREVERSE
#define BITREVERSE

#include <map>

class BitReverse
{
public:
	static std::map<int, int*> Tables;
	static std::map<int, int> Bitsize;

	static void Setup();
	static void Generate(int size);
	static int Reverse(int input, int bits);
};

#endif
