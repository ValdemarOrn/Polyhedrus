#ifndef AUDIOLIB_SVFFILTER
#define AUDIOLIB_SVFFILTER

#include "MathDefs.h"
#include "Utils.h"
#include <cmath>

namespace AudioLib
{
	class SvfFilter
	{
	public:
		float Fc;
		float Resonance;
		float Fs;

		float Lp;
		float Bp;
		float Hp;
		float No;

		inline SvfFilter()
		{
			Fc = 0.5;
			Resonance = 0.5;
			f = 0.2;
			d = 1.0;

			Lp = 0.0;
			Bp = 0.0;
			Hp = 0.0;
			No = 0.0;
			zState1 = 0.0;
			zState2 = 0.0;
		}

		inline void Update()
		{
			Resonance = Utils::Limit(Resonance, 0, 1.0f);
			f = 2 * std::sin(M_PI * Fc / Fs);
			d = (1 - (Resonance * 0.999)) * 2;
		}

		inline void Process(float x)
		{
			Lp = zState2 + f * zState1;
			Hp = x - Lp - d * zState1;
			Bp = f * Hp + zState1;
			No = Hp + Lp;

			zState1 = Bp;
			zState2 = Lp;

		}

	private:
		float f;
		float d;

		float zState1;
		float zState2;
	};
}

#endif