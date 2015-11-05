#include "Envelope.h"

namespace Leiftur
{
	const float Envelope::MaxTimeSeconds = 30.0;

	Envelope::Envelope()
	{
		output = 0.0;
		Velocity = 1.0;
		VelocityAmount = 0.0;
		Gate = false;
		section = SectionPostRelease;
		iterator = 0.0;

		attackInc = 0.01;
		holdInc = 0.01;
		decayInc = 0.01;
		sustain = 0.5;
		releaseInc = 0.01;
	}

	Envelope::~Envelope()
	{
	}

	void Envelope::Initialize(int samplerate)
	{
		this->samplerate = samplerate;
	}

	void Envelope::SetParameter(EnvParameters parameter, double value)
	{
		auto transform = [=](float input) 
		{ 
			float sampleLength = GetDecayTime(input) * samplerate;
			return 1.0 / (sampleLength + 1);
		};

		switch (parameter)
		{
		case EnvParameters::Attack:
			attack = value;
			attackInc = transform(value);
			break;
		case EnvParameters::Hold:
			hold = value;
			if (value == 0)
				holdInc = 1.0;
			else
				holdInc = transform(value);
			break;
		case EnvParameters::Decay:
			decay = value;
			decayInc = transform(value);
			break;
		case EnvParameters::Sustain:
			sustain = value;
			break;
		case EnvParameters::Release:
			release = value;
			releaseInc = transform(value);
			break;
		case EnvParameters::Velocity:
			VelocityAmount = value;
			break;
		}
	}

	float Envelope::Process(int samples)
	{
		if (Gate && section >= SectionRelease)
		{
			section = SectionAttack;
			iterator = sqrt(output);
		}
		else if (!Gate && section < SectionRelease)
		{
			section = SectionRelease;
			iterator = sqrt(output);
		}

		switch (section)
		{
		case SectionAttack:
			iterator += attackInc * samples;
			iterator = (iterator < 1.0) ? iterator : 1.0;
			output = iterator * iterator;
			if (iterator >= 1.0)
			{
				iterator = 0.0;
				section++;
			}
			break;
		case SectionHold:
			iterator += holdInc * samples;
			iterator = (iterator < 1.0) ? iterator : 1.0;
			output = 1.0;
			if (iterator >= 1.0)
			{
				iterator = 1.0;
				section++;
			}
			break;
		case SectionDecay:
			iterator -= decayInc * samples;
			iterator = (iterator > 0.0) ? iterator : 0.0;
			output = sustain + (iterator * iterator) * (1.0 - sustain);
			if (iterator <= 0.0)
			{
				iterator = 0.0;
				section++;
			}
			break;
		case SectionSustain:
			output = sustain;
			break;
		case SectionRelease:
			iterator -= releaseInc * samples;
			iterator = (iterator > 0.0) ? iterator : 0.0;
			output = iterator * iterator;
			if (iterator <= 0.0)
			{
				iterator = 0.0;
				section++;
			}
			break;
		case SectionPostRelease:
			iterator = 0;
			output = 0.0;
			break;
		}

		return GetOutput();
	}

	void Envelope::Silence()
	{
		iterator = 0;
		section = SectionPostRelease;
	}

	void Envelope::Reset()
	{
		iterator = 0;
		section = 0;
	}

	std::vector<uint8_t> Envelope::GetVisual()
	{
		std::vector<uint8_t> output;
		int len = 100;

		output.push_back(0);

		int stageLen = attack * len;
		for (int i = 0; i < stageLen; i++)
		{
			float fval = i / (float)stageLen;
			uint8_t val = (int)(fval * 255.999);
			output.push_back(val);
		}

		output.push_back(255);

		stageLen = hold * len;
		for (int i = 0; i < stageLen; i++)
		{
			output.push_back(255);
		}

		stageLen = decay * len;
		for (int i = 0; i < stageLen; i++)
		{
			float fval = (1 - i / (float)stageLen) * (1 - sustain) + sustain;
			uint8_t val = (int)(fval * 255.999);
			output.push_back(val);
		}

		stageLen = len / 2;
		for (int i = 0; i < stageLen; i++)
		{
			float fval = sustain;
			uint8_t val = (int)(fval * 255.999);
			output.push_back(val);
		}

		stageLen = release * len;
		for (int i = 0; i < stageLen; i++)
		{
			float fval = sustain * (1 - (i / (float)stageLen));
			uint8_t val = (int)(fval * 255.999);
			output.push_back(val);
		}

		return output;
	}

}
