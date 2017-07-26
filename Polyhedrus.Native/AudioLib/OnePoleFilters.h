#ifndef AUDIOLIB_ONEPOLEFILTERS
#define AUDIOLIB_ONEPOLEFILTERS

#include "MathDefs.h"
#include <cmath>

namespace AudioLib
{
	class Lp1
	{
	private:
		float z1_state;
		//float g;
		float g2;
	public:
		Lp1()
		{
			z1_state = 0;
			g2 = 0;
		}

		inline float Process(float x)
		{
			// perform one sample tick of the lowpass filter
			//float v = (x - z1_state) * g / (1 + g);
			float v = (x - z1_state) * g2;
			float y = v + z1_state;
			z1_state = y + v;
			return y;
		}

		inline float ProcessNoUpdate(float x)
		{
			// perform one sample tick of the lowpass filter
			//float v = (x - z1_state) * g / (1 + g);
			float v = (x - z1_state) * g2;
			return v + z1_state;
		}

		// 0...1
		inline void SetFc(float fcRel)
		{
			//this->g = fcRel * M_PI;
			float g = (float)(fcRel * M_PI);
			g2 = g / (1 + g);
		}
	};

	class Hp1
	{
	private:
		float z1_state;
		//float g;
		float g2;
	public:
		Hp1()
		{
			z1_state = 0;
			g2 = 0;
		}

		inline float Process(float x)
		{
			// perform one sample tick of the lowpass filter
			//float v = (x - z1_state) * g / (1 + g);
			float v = (x - z1_state) * g2;
			float y = v + z1_state;
			z1_state = y + v;
			return x - y;
		}

		// 0...1
		inline void SetFc(float fcRel)
		{
			//this->g = fcRel * M_PI;
			float g = (float)(fcRel * M_PI);
			g2 = g / (1 + g);
		}
	};
}

#endif

