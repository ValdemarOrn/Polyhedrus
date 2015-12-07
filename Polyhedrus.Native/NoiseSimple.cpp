#include "NoiseSimple.h"

namespace Polyhedrus
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
		bufferL = new float[bufferSize]();
		bufferR = new float[bufferSize]();
		output[0] = bufferL;
		output[1] = bufferR;

		lpL.SetFc(0.01f);
		lpR.SetFc(0.01f);
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
				float rl = (2.0f * random.NextFloat() - 1.0f);
				float rr = (2.0f * random.NextFloat() - 1.0f);
				L -= L * hpRolloff;
				R -= R * hpRolloff;
				L += rl;
				R += rr;
				
				if (L > 32 || L < -32) L -= rl;
				if (R > 32 || R < -32) R -= rr;

				bufferL[i] = 0.03125f * L;
				bufferR[i] = 0.03125f * R;
			}
		}
		else if (Type == NoiseType::Pink)
		{
			for (int i = 0; i < bufferSize; i++)
			{
				bufferL[i] = lpL.Process(2.0f * random.NextFloat() - 1.0f);
				bufferR[i] = lpR.Process(2.0f * random.NextFloat() - 1.0f);
			}
		}
	}
}
