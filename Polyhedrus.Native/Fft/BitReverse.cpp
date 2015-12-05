#include "BitReverse.h"

std::map<int, int*> BitReverse::Tables;
std::map<int, int> BitReverse::Bitsize;

void BitReverse::Setup()
{
	BitReverse::Bitsize[4] = 2;
	BitReverse::Bitsize[8] = 3;
	BitReverse::Bitsize[16] = 4;
	BitReverse::Bitsize[32] = 5;
	BitReverse::Bitsize[64] = 6;
	BitReverse::Bitsize[128] = 7;
	BitReverse::Bitsize[256] = 8;
	BitReverse::Bitsize[512] = 9;
	BitReverse::Bitsize[1024] = 10;
	BitReverse::Bitsize[2048] = 11;
	BitReverse::Bitsize[4096] = 12;
	BitReverse::Bitsize[8192] = 13;
	BitReverse::Bitsize[16384] = 14;
	BitReverse::Bitsize[32768] = 15;
	BitReverse::Bitsize[65536] = 16;

	int i = 4;
	while (i <= 65536)
	{
		Generate(i);
		i = i * 2;
	}
}

void BitReverse::Generate(int size)
{
	int bits = Bitsize[size];
	int* table = new int[size];
	Tables[size] = table;

	for(int i = 0; i < size; i++)
	{
		table[i] = Reverse(i, bits);
	}
}

int BitReverse::Reverse(int input, int bits)
{
	int output = 0;
	for(int i = 0; i < bits; i++)
	{
		int mask = 1 << i;
		int outmask = 1 << (bits - 1 - i);
		if ((mask & input) > 0)
			output += outmask;
	}

	return output;
}
