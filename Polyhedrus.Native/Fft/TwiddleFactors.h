
#ifndef TWIDDLE
#define TWIDDLE

#include "Complex.h"
#include <map>

template <class TVal>
class TwiddleFactors
{
public:
	static std::map<int, Complex<TVal>*> Factors;
	static void Setup();
};


// ------------ Implementation ------------

#define PI 3.1415926535897932384626433832795

template <class TVal>
std::map<int, Complex<TVal>*> TwiddleFactors<TVal>::Factors;

template <class TVal>
void TwiddleFactors<TVal>::Setup()
{
	int N = 65536;
	Complex<TVal>* masterArray = new Complex<TVal>[N/2];

	for(int i = 0; i < N/2; i++)
	{
		masterArray[i] = Complex<TVal>::CExp((TVal)(-i * 2 * PI / N));
	}

	TwiddleFactors<TVal>::Factors[N] = masterArray;

	int hop = 1;
	while (N > 2)
	{
		hop = hop * 2;
		N = N / 2;

		Complex<TVal>* arr = new Complex<TVal>[N/2];
		TwiddleFactors<TVal>::Factors[N] = arr;

		for (int i = 0; i < N/2; i++)
			arr[i] = masterArray[i * hop];
	}
}

#endif