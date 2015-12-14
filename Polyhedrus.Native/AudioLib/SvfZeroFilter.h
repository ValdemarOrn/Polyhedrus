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
					
			Lp = 0.0f;
			Bp = 0.0f;
			Hp = 0.0f;
		}

		inline void Update()
		{
			float fcRel = Fc / Fs;
			g = (float)(fcRel * M_PI);

			R = 1 - Resonance;
		}

		inline void SetFc(float fcRel)
		{
			Fc = fcRel;
			g = (float)(fcRel * M_PI);
		}

		inline void ProcessLinear(float x)
		{
			//Hp = (x - 2 * R * s1 - g * s1 - s2) / (1 + 2 * R * g + g * g);
			Integrator1();
			Integrator2();
		}

	private:
		
		inline void Integrator1()
		{
			float y = Bp;
			float x = Hp;
			float z = s1;

			// compute input to z
			float u = y + g * x;

			// Compute new output
			y = z + g * x;

			// assign to variables
			s1 = u;
			Bp = y;
		}

		inline void Integrator2()
		{
			float y = Lp;
			float x = Bp;
			float z = s2;

			// compute input to z
			float u = y + g * x;

			// Compute new output
			y = z + g * x;

			// assign to variables
			s2 = u;
			Lp = y;
		}
	};
}

#endif