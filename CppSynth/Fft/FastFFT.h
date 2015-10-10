#ifndef FASTFFT
#define FASTFFT

#include "Complex.h"
#include "BitReverse.h"
#include "TwiddleFactors.h"

template <class TVal>
class FastFFT
{
public:
	static void Setup();
	static void IFFT(const Complex<TVal>* input, Complex<TVal>* output, Complex<TVal>* scratchpad, const int len);
	static void FFT(const Complex<TVal>* input, Complex<TVal>* output, Complex<TVal>* scratchpad, const int len);

	static void Swap(Complex<TVal>** inp, Complex<TVal>** outp);
	static void Butterfly2(const Complex<TVal>* inp, Complex<TVal>* outp, const int len);
	static void Butterfly4(const Complex<TVal>* inp, Complex<TVal>* outp, const int len);
	static void Butterfly8(const Complex<TVal>* inp, Complex<TVal>* outp, const int len);
	static void Butterfly(const Complex<TVal>* inp, Complex<TVal>* outp, const int stageSize, const int len);
};

/// ----- Implementation -----

template <class TVal>
inline void FastFFT<TVal>::Setup()
{
	BitReverse::Setup();
	TwiddleFactors<float>::Setup();
	TwiddleFactors<double>::Setup();
}

template <class TVal>
inline void FastFFT<TVal>::IFFT(const Complex<TVal>* input, Complex<TVal>* output, Complex<TVal>* scratchpad, const int len)
{
	TVal scale = (TVal)(1.0 / len);
	TVal scaleNeg = -scale;

	TVal* scratchPtr = (TVal*)scratchpad;
	TVal* inputPtr = (TVal*)input;

	for (int i = 0; i < len * 2; i += 2)
	{
		scratchPtr[i] = inputPtr[i] * scale;
		scratchPtr[i + 1] = inputPtr[i + 1] * scaleNeg;
	}

	FFT(scratchpad, output, scratchpad, len);
}

template <class TVal>
inline void FastFFT<TVal>::FFT(const Complex<TVal>* input, Complex<TVal>* output, Complex<TVal>* scratchpad, const int len)
{
	if (len == 1)
	{
		output[0] = input[0];
		return;
	}
	else if (len == 2)
	{
		output[0] = input[0];
		output[1] = input[0];

		Complex<TVal>::Add(output[0], input[1]);
		Complex<TVal>::Subtract(output[1], input[1]);
		return;
	}

	// we use the output buffer and another buffer called scratchpad to work on the
	// signals. Input signal never gets modified
	Complex<TVal>* A = output;
	Complex<TVal>* B = scratchpad;

	int* bitMap = BitReverse::Tables[len];

	for (int i = 0; i < len; i++)
		A[i] = input[bitMap[i]];

	Butterfly2(A, B, len);
	Swap(&A, &B);
	Butterfly4(A, B, len);

	if (len == 4)
		goto end;

	Swap(&A, &B);
	Butterfly8(A, B, len);

	int butterflySize = 8;

	while (true)
	{
		butterflySize *= 2;

		if (butterflySize > len)
			break;

		Swap(&A, &B);
		Butterfly(A, B, butterflySize, len);
	}

end:

	// copy output to the correct buffer
	if (B != output)
	{
		for (int i = 0; i < len; i++)
			output[i] = B[i];
	}
}

template <class TVal>
inline void FastFFT<TVal>::Swap(Complex<TVal>** inp, Complex<TVal>** outp)
{
	Complex<TVal>* temp = *inp;
	*inp = *outp;
	*outp = temp;
}

template <class TVal>
inline void FastFFT<TVal>::Butterfly2(const Complex<TVal>* inp, Complex<TVal>* outp, const int len)
{
	for (int i = 0; i < len; i += 2)
	{
		outp[i] = inp[i];
		outp[i + 1] = inp[i];

		Complex<TVal>::Add(outp[i], inp[i + 1]);
		Complex<TVal>::Subtract(outp[i + 1], inp[i + 1]);
	}
}

template <class TVal>
inline void FastFFT<TVal>::Butterfly4(const Complex<TVal>* inp, Complex<TVal>* outp, const int len)
{
	Complex<TVal>* w = TwiddleFactors<TVal>::Factors[4];

	for (int i = 0; i < len; i += 4)
	{
		outp[i] = inp[i];
		outp[i + 2] = inp[i];

		outp[i + 1] = inp[i + 1];
		outp[i + 3] = inp[i + 1];

		Complex<TVal> x0, x1;
		x0 = inp[i + 2]; //Complex<TVal>::Multiply(x0, inp[i + 2], w[0]);
		Complex<TVal>::Multiply(x1, inp[i + 3], w[1]);

		Complex<TVal>::Add(outp[i], outp[i], x0);
		Complex<TVal>::Add(outp[i + 1], outp[i + 1], x1);
		Complex<TVal>::Subtract(outp[i + 2], outp[i + 2], x0);
		Complex<TVal>::Subtract(outp[i + 3], outp[i + 3], x1);
	}
}

template <class TVal>
inline void FastFFT<TVal>::Butterfly8(const Complex<TVal>* inp, Complex<TVal>* outp, const int len)
{
	Complex<TVal>* w = TwiddleFactors<TVal>::Factors[8];

	for (int i = 0; i < len; i += 8)
	{
		outp[i] = inp[i];
		outp[i + 1] = inp[i + 1];
		outp[i + 2] = inp[i + 2];
		outp[i + 3] = inp[i + 3];

		outp[i + 4] = inp[i];
		outp[i + 5] = inp[i + 1];
		outp[i + 6] = inp[i + 2];
		outp[i + 7] = inp[i + 3];

		Complex<TVal> x0, x1, x2, x3;
		x0 = inp[i + 4]; //Complex<TVal>::Multiply(x0, inp[i + 4], w[0]);
		Complex<TVal>::Multiply(x1, inp[i + 5], w[1]);
		Complex<TVal>::Multiply(x2, inp[i + 6], w[2]);
		Complex<TVal>::Multiply(x3, inp[i + 7], w[3]);

		Complex<TVal>::Add(outp[i], outp[i], x0);
		Complex<TVal>::Add(outp[i + 1], outp[i + 1], x1);
		Complex<TVal>::Add(outp[i + 2], outp[i + 2], x2);
		Complex<TVal>::Add(outp[i + 3], outp[i + 3], x3);
		Complex<TVal>::Subtract(outp[i + 4], outp[i + 4], x0);
		Complex<TVal>::Subtract(outp[i + 5], outp[i + 5], x1);
		Complex<TVal>::Subtract(outp[i + 6], outp[i + 6], x2);
		Complex<TVal>::Subtract(outp[i + 7], outp[i + 7], x3);
	}
}

template <class TVal>
inline void FastFFT<TVal>::Butterfly(const Complex<TVal>* inp, Complex<TVal>* outp, const int stageSize, const int len)
{
	Complex<TVal>* w = TwiddleFactors<TVal>::Factors[stageSize];
	const int s2 = stageSize / 2;

	for (int n = 0; n < len; n += stageSize)
	{
		const Complex<TVal>* lower = &inp[n];
		const Complex<TVal>* upper = &inp[n + s2];
		Complex<TVal>* outLower = &outp[n];
		Complex<TVal>* outhigher = &outp[n + s2];

		for (int i = 0; i < s2; i++)
		{
			outLower[i] = lower[i];
			outhigher[i] = lower[i];
		}

		for (int i = 0; i < s2; i++)
		{
			Complex<TVal> x;
			Complex<TVal>::Multiply(x, upper[i], w[i]);

			Complex<TVal>::Add(outLower[i], x);
			Complex<TVal>::Subtract(outhigher[i], x);
		}
	}
}


#endif
