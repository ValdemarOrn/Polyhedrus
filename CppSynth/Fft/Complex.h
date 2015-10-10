
#ifndef COMPLEX
#define COMPLEX

#pragma pack(push, 4)
template <class TVal>
class Complex
{
public:
	TVal Real;
	TVal Imag;

	Complex();
	Complex(const TVal real, const TVal imag);

	static Complex CExp(const TVal phase);
	static void Multiply(Complex& dest, const Complex& c1, const Complex& c2);
	static void Add(Complex& dest, const Complex& c1, const Complex& c2);
	static void Subtract(Complex& dest, const Complex& c1, const Complex& c2);

	static void Multiply(Complex& dest, const Complex& c1);
	static void Add(Complex& dest, const Complex& c1);
	static void Subtract(Complex& dest, const Complex& c1);

	static const Complex I;
};
#pragma pack(pop)

// ------------ Implementation ------------

#include<cmath>

template <class TVal>
const Complex<TVal> Complex<TVal>::I = Complex<TVal>(0, 1);

template <class TVal>
inline Complex<TVal>::Complex()
{
	Real = 0;
	Imag = 0;
}

template <class TVal>
inline Complex<TVal>::Complex(const TVal real, const TVal imag)
{
	Real = real;
	Imag = imag;
}

template <class TVal>
inline Complex<TVal> Complex<TVal>::CExp(const TVal phase)
{
	TVal x = cos(phase);
	TVal y = sin(phase);
	return Complex<TVal>(x, y);
}


// ------------- fast operations -------------

template <class TVal>
inline void Complex<TVal>::Multiply(Complex<TVal>& dest, const Complex<TVal>& c1, const Complex<TVal>& c2)
{
	TVal r = c1.Real * c2.Real - c1.Imag * c2.Imag;
	TVal i = c1.Real * c2.Imag + c1.Imag * c2.Real;
	dest.Real = r;
	dest.Imag = i;
}

template <class TVal>
inline void Complex<TVal>::Add(Complex& dest, const Complex& c1, const Complex& c2)
{
	dest.Real = c1.Real + c2.Real;
	dest.Imag = c1.Imag + c2.Imag;
}

template <class TVal>
inline void Complex<TVal>::Subtract(Complex& dest, const Complex& c1, const Complex& c2)
{
	dest.Real = c1.Real - c2.Real;
	dest.Imag = c1.Imag - c2.Imag;
}


template <class TVal>
inline void Complex<TVal>::Multiply(Complex<TVal>& dest, const Complex<TVal>& c1)
{
	TVal r = dest.Real * c1.Real - dest.Imag * c1.Imag;
	dest.Imag = dest.Real * c1.Imag + dest.Imag * c1.Real;
	dest.Real = r;
}

template <class TVal>
inline void Complex<TVal>::Add(Complex& dest, const Complex& c1)
{
	dest.Real += c1.Real;
	dest.Imag += c1.Imag;
}

template <class TVal>
inline void Complex<TVal>::Subtract(Complex& dest, const Complex& c1)
{
	dest.Real -= c1.Real;
	dest.Imag -= c1.Imag;
}


#endif
