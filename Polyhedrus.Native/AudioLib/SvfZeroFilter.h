#ifndef AUDIOLIB_SVF_ZERO_FILTER
#define AUDIOLIB_SVF_ZERO_FILTER

#include "MathDefs.h"
#include "Utils.h"
#include <cmath>

namespace AudioLib
{
	// Sources:
	// https://www.native-instruments.com/fileadmin/ni_media/downloads/pdf/VAFilterDesign_1.1.1.pdf
	class SvfZeroFilter
	{
	public:
		float Fc;
		float Resonance;
		float Fs;

		float Lp;
		float Bp;
		float Hp;

	private:
		float g;
		float s1, s2;
		float R;
		float divisor;

	public:

		inline SvfZeroFilter()
		{
			Fc = 0.5f;
			Resonance = 0.5f;
			Fs = 48000.0f;

			g = 0.5f;
			R = 1.0f;
			s1 = 0.0f;
			s2 = 0.0f;
			divisor = 1.0f;
					
			Lp = 0.0f;
			Bp = 0.0f;
			Hp = 0.0f;
		}

		inline void Update()
		{
			float fcRel = Fc / Fs;
			g = (float)(fcRel * M_PI);

			R = 1 - Resonance;
			divisor = 1.0f / (1 + 2 * R * g + g * g);
		}

		inline void SetFc(float fcRel)
		{
			Fc = fcRel * 2; // OK, I've no idea why I need that x2 factor, but without it, the resonance freq. is just an octave too low!!
			g = (float)(fcRel * M_PI);
		}

		inline void Process(float x)
		{
			//Hp = (x - 2 * R * s1 - g * s1 - s2) / (1 + 2 * R * g + g * g);
			Hp = (x - 2 * R * s1 - g * s1 - s2) * divisor;
			
			// Integrator 1

			// store temp
			float z = s1;
			// compute input to z
			s1 = Bp + g * Hp;
			// Compute new output
			Bp = z + g * Hp;
			
			// Integrator 2
			
			// store temp
			z = s2;
			// compute input to z
			s2 = Lp + g * Bp;
			// Compute new output
			Lp = z + g * Bp;
		}

		inline void ProcessHp(float* input, float* output, int len)
		{
			register float hp = Hp;
			register float lp = Lp;
			register float bp = Bp;
			register float ss1 = s1;
			register float ss2 = s2;
			register float r = R;
			register float gg = g;
			register float divi = divisor;
			register float z;

			for (int i = 0; i < len; i++)
			{
				hp = (input[i] - 2 * r * ss1 - gg * ss1 - ss2) * divi;

				// Integrator 1
				z = ss1;
				ss1 = bp + gg * hp;
				bp = z + gg * hp;

				// Integrator 2
				z = ss2;
				ss2 = lp + gg * bp;
				lp = z + gg * bp;

				output[i] = hp;
			}
			
			Hp = hp;
			Lp = lp;
			Bp = bp;
			s1 = ss1;
			s2 = ss2;
		}
	};
}

#endif