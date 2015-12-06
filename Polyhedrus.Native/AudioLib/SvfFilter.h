#ifndef AUDIOLIB_SVFFILTER
#define AUDIOLIB_SVFFILTER

#include "MathDefs.h"
#include "Utils.h"
#include <cmath>

namespace AudioLib
{
	// Sources:
	// https://www.zhdk.ch/fileadmin/data_subsites/data_icst/Downloads/Digital_Sound/Digital_Sound_Generation_2.pdf
	// http://musicdsp.org/showone.php?id=142
	// It is HIGHLY recommended to oversample this filter by at least 2x
	class SvfFilter
	{
	public:
		float Fc;
		float Resonance;
		float Fs;
		bool Nonlinear;

		float Lp;
		float Bp;
		float Hp;
		float No;

		inline SvfFilter()
		{
			Nonlinear = false;
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
			// usually the parameter Q is used, and then Q = [0....inf[
			// and then D = 1 / Q. for Q >= 0.5, this becomes a range [0...2]
			Resonance = Utils::Limit(Resonance, 0, 1.0f);
			d = (1 - Resonance) * 2;
									
			f = 2 * std::sin(M_PI * Fc / Fs);
			//f = f * (1.85 - 0.85 * d * f);

			// adjustment factors from paper
			d = std::fmin(d, 2 - f);
		}

		inline void ProcessLinear(float x)
		{
			Lp = zState2 + f * zState1;
			Hp = x - Lp - d * zState1;
			Bp = f * Hp + zState1;
			No = Hp + Lp;

			zState1 = Bp;
			zState2 = Lp;
		}

		inline void ProcessNonlinear(float x)
		{
			Lp = zState2 + f * zState1;
			Hp = x - Lp - d * zState1;
			Bp = f * Hp + zState1;
			No = Hp + Lp;

			zState1 = AudioLib::Utils::Cubic6Nonlin(Bp);
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