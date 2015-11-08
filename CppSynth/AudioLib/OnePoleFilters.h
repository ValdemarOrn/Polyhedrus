#ifndef AUDIOLIB_ONEPOLEFILTERS
#define AUDIOLIB_ONEPOLEFILTERS

#define _USE_MATH_DEFINES
#include <cmath>

namespace AudioLib
{
	class Lp1
	{
	private:
		float z1_state;
		float g;
	public:
		inline float Process(float x)
		{
			// perform one sample tick of the lowpass filter
			float v = (x - z1_state) * g / (1 + g);
			float y = v + z1_state;
			z1_state = y + v;
			return y;
		}

		inline void SetFc(float fcRel)
		{
			this->g = fcRel * M_PI;
		}
	};

	class Hp1
	{
	private:
		float z1_state;
		float g;
	public:
		inline float Process(float x)
		{
			// perform one sample tick of the lowpass filter
			float v = (x - z1_state) * g / (1 + g);
			float y = v + z1_state;
			z1_state = y + v;
			return x - y;
		}

		inline void SetFc(float fcRel)
		{
			this->g = fcRel * M_PI;
		}
	};
}

#endif

