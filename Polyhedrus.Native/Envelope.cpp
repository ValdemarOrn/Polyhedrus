#include "Envelope.h"
#include "AudioLib/Utils.h"
#include <cmath>

using namespace std;

namespace Polyhedrus
{
	const float Envelope::MaxTimeSeconds = 30.0;

	Envelope::Envelope()
	{
		output = 0.0;
		velocity = 1.0;
		VelocityAmount = 0.0;
		Retrigger = false;
		gate = false;
		section = SectionPostRelease;
		iterator = 0.0;

		attackInc = 0.01f;
		holdInc = 0.01f;
		decayInc = 0.01f;
		sustain = 0.5f;
		releaseInc = 0.01f;

		CreateCurve(attackCurve, 0.0);
		CreateCurve(decayCurve, 0.0);
		CreateCurve(releaseCurve, 0.0);
		CreateCurve(velocityCurve, 0.0);
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
		auto transform = [=](double input) 
		{ 
			float sampleLength = GetDecayTime((float)input) * samplerate;
			return 1.0f / (sampleLength + 1);
		};

		switch (parameter)
		{
		case EnvParameters::Attack:
			attack = (float)value;
			attackInc = transform(value);
			break;
		case EnvParameters::Hold:
			hold = (float)value;
			if (value == 0)
				holdInc = 1.0;
			else
				holdInc = transform(value);
			break;
		case EnvParameters::Decay:
			decay = (float)value;
			decayInc = transform(value);
			break;
		case EnvParameters::Sustain:
			sustain = (float)value;
			break;
		case EnvParameters::Release:
			release = (float)value;
			releaseInc = transform(value);
			break;
		case EnvParameters::Velocity:
			VelocityAmount = (float)value;
			break;
		case EnvParameters::AttackCurve:
			CreateCurve(attackCurve, value);
			break;
		case EnvParameters::DecayCurve:
			CreateCurve(decayCurve, value);
			break;
		case EnvParameters::ReleaseCurve:
			CreateCurve(releaseCurve, value);
			break;
		case EnvParameters::VelocityCurve:
			CreateCurve(velocityCurve, value);
			break;
		case EnvParameters::Retrigger:
			Retrigger = value >= 0.5;
			break;
		}
	}

	float Envelope::Process(int samples)
	{
		if (gate && section >= SectionRelease)
		{
			section = SectionAttack;
			attackLevel = output;
			iterator = 0;
		}
		else if (!gate && section < SectionRelease)
		{
			section = SectionRelease;
			releaseLevel = output;
			iterator = 1;
		}

		switch (section)
		{
		case SectionAttack:
			iterator += attackInc * samples;
			iterator = (iterator < 1.0) ? iterator : 1.0f;
			output = attackLevel + Lookup(attackCurve, iterator) * (1 - attackLevel);
			if (iterator >= 1.0)
			{
				iterator = 0.0;
				section++;
			}
			break;
		case SectionHold:
			iterator += holdInc * samples;
			iterator = (iterator < 1.0) ? iterator : 1.0f;
			output = 1.0;
			if (iterator >= 1.0)
			{
				iterator = 1.0;
				section++;
			}
			break;
		case SectionDecay:
			iterator -= decayInc * samples;
			iterator = (iterator > 0.0) ? iterator : 0.0f;
			output = sustain + Lookup(decayCurve, iterator) * (1.0f - sustain);
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
			iterator = (iterator > 0.0) ? iterator : 0.0f;
			output = Lookup(releaseCurve, iterator) * releaseLevel;
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

	void Envelope::SetGate(bool gate, float velocity)
	{
		this->gate = gate;

		if (gate)
		{
			this->velocity = AudioLib::Utils::LinInterp(velocityCurve, 200, velocity);
			if (Retrigger)
				Reset();
		}
	}

	void Envelope::Silence()
	{
		iterator = 0;
		section = SectionPostRelease;
		output = 0;
	}

	void Envelope::Reset()
	{
		iterator = 0;
		section = 0;
		attackLevel = 0;
	}

	std::vector<uint8_t> Envelope::GetVisual()
	{
		std::vector<uint8_t> output;
		int len = 100;

		int stageLen = (int)(attack * len);
		if (stageLen == 0)
			output.push_back(0);

		for (int i = 0; i < stageLen; i++)
		{
			float fval = i / (float)stageLen;
			fval = Lookup(attackCurve, fval);
			uint8_t val = (int)(fval * 255.999);
			output.push_back(val);
		}

		stageLen = (int)(hold * len);
		if (stageLen == 0 && (int)(decay * len) == 0)
			output.push_back(255);

		for (int i = 0; i < stageLen; i++)
		{
			output.push_back(255);
		}

		stageLen = (int)(decay * len);
		for (int i = 0; i < stageLen; i++)
		{
			float fval = (1 - i / (float)stageLen);
			fval = Lookup(decayCurve, fval) * (1 - sustain) + sustain;

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

		stageLen = (int)(release * len);
		for (int i = 0; i < stageLen; i++)
		{
			float fval = (1 - i / (float)stageLen);
			fval = Lookup(releaseCurve, fval) * sustain;

			uint8_t val = (int)(fval * 255.999);
			output.push_back(val);
		}

		if (output.at(output.size() - 1) != 0)
			output.push_back(0);

		return output;
	}

	void Envelope::CreateCurve(float* table, double expo)
	{
		expo = AudioLib::Utils::Limit((float)expo, -1.0f, 1.0f);
		int N = TableSize;

		if (abs(expo) < 0.001)
			expo = 0.001;

		double expAbs = abs(expo);
		bool isNeg = expo < 0.0;
		double min = pow(10, -2 * expAbs);
		double decayN = std::log2(min);
		double decay = -decayN / N;

		double minVal = pow(2, -decay * (N - 1));
		double scale = 1 - minVal;

		for (int i = 0; i < N; i++)
		{
			if (isNeg)
				table[i] = (float)((pow(2, -decay * (N - i - 1)) - minVal) / scale);
			else
				table[i] = (float)(1 - (pow(2, -decay * i) - minVal) / scale);
		}
	}

}
