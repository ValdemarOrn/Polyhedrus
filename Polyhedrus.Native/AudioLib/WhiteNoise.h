#pragma once

namespace AudioLib
{
	class WhiteNoise
	{
	private:

		// http://musicdsp.org/archive.php?classid=1#216
		const float g_fScale = 2.0f / 0xffffffff;
		int g_x1 = 0x67452301;
		int g_x2 = 0xefcdab89;

	public:

		void Sample(float* buffer, unsigned int bufferSize)
		{
			while (bufferSize--)
			{
				g_x1 ^= g_x2;
				*buffer++ = g_x2 * g_fScale;
				g_x2 += g_x1;
			}
		}

		void Cycle(unsigned int count)
		{
			while (count--)
			{
				g_x1 ^= g_x2;
				g_x2 += g_x1;
			}
		}
	};
}
