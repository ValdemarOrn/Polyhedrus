#include "NoiseSimple.h"
#include "AudioLib/Utils.h"

namespace Polyhedrus
{
	NoiseSimple::NoiseSimple()
	{
		lpL.SetFc(0.01f);
		lpR.SetFc(0.01f);
		hpL.SetFc(10.0f / 22000.0f);
		hpR.SetFc(10.0f / 22000.0f);
		bufferL = 0;
		bufferR = 0;
		brownL = 0.0f;
		brownR = 0.0f;
	}

	NoiseSimple::~NoiseSimple()
	{
		delete bufferL;
		delete bufferR;
	}

	void NoiseSimple::Initialize(int samplerate, int bufferSize)
	{
		this->samplerate = samplerate;
		this->bufferSize = bufferSize;
		
		bufferL = new float[bufferSize]();
		bufferR = new float[bufferSize]();
		output[0] = bufferL;
		output[1] = bufferR;
	}

	void NoiseSimple::Process(int len)
	{
		float volWhite = AudioLib::Utils::Limit(1.0f - Type, 0.0f, 1.0f);
		float volBrown = AudioLib::Utils::Limit(Type - 1.0f, 0.0f, 1.0f);
		float volPink = (1.0f - volWhite - volBrown) * 3.0f;
		volBrown *= 0.15f;

		for (int i = 0; i < len; i++)
		{
			float randL = 2 * random.NextFloat() - 1.0f;
			float randR = 2 * random.NextFloat() - 1.0f;
			
			brownL += randL;
			brownR += randR;
			float bl = hpL.Process(brownL);
			float br = hpR.Process(brownR);

			float pl = lpL.Process(randL);
			float pr = lpR.Process(randR);
			
			bufferL[i] = volWhite * randL + volPink * pl + volBrown * bl;
			bufferR[i] = volWhite * randR + volPink * pr + volBrown * br;
		}
	}
}
