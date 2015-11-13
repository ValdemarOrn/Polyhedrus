#include "NoiseSimple.h"

namespace Leiftur
{
	NoiseSimple::NoiseSimple()
	{
		lpL.SetFc(0.01f);
		lpR.SetFc(0.01f);
		bufferL = 0;
		bufferR = 0;
		L = 0.0f;
		R = 0.0f;
		hpRolloff = 0.001f;
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
		bufferL = new float[bufferSize];
		bufferR = new float[bufferSize];
		output[0] = bufferL;
		output[1] = bufferR;
	}



	void NoiseSimple::Process(int len)
	{
		if (Type == NoiseType::White)
		{
			for (int i = 0; i < bufferSize; i++)
			{
				bufferL[i] = random.NextFloat();
				bufferR[i] = random.NextFloat();
			}
		}
		else if (Type == NoiseType::Brown)
		{
			for (int i = 0; i < bufferSize; i++)
			{
				float rl = (2 * random.NextFloat() - 1);
				float rr = (2 * random.NextFloat() - 1);
				L -= L * hpRolloff;
				R -= R * hpRolloff;
				L += rl;
				R += rr;
				
				if (L > 32 || L < -32) L -= rl;
				if (R > 32 || R < -32) R -= rr;

				bufferL[i] = 0.03125 * L;
				bufferR[i] = 0.03125 * R;
			}
		}
		else if (Type == NoiseType::Pink)
		{
			lpL.SetFc(0.01);
			lpR.SetFc(0.01);
			for (int i = 0; i < bufferSize; i++)
			{
				bufferL[i] = lpL.Process(random.NextFloat()) * 2;
				bufferR[i] = lpR.Process(random.NextFloat()) * 2;
			}
		}
	}
}
