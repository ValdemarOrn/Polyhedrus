#include "Character.h"
#include "AudioLib/Utils.h"

using AudioLib::Utils;

namespace Leiftur
{
	Character::Character()
		: bqBottom(AudioLib::Biquad::FilterType::LowShelf, 48000)
		, bqTop(AudioLib::Biquad::FilterType::HighShelf, 48000)
	{
		Note = 60;
		biquadUpdateCounter = 0;
		glideRate = 1;
		buffer = 0;
		decimateCounter = 0;
		lastDecimateVal = 0;
		bqBottom.SetQ(1);
		bqTop.SetQ(1);
		bqBottom.SetGain(1);
		bqTop.SetGain(1);
		bqBottom.Slope = 1;
		bqTop.Slope = 1;

		bqBottom.Frequency = 200;
		bqTop.Frequency = 4000;
		bqBottom.Update();
		bqTop.Update();
	}

	Character::~Character()
	{
		delete buffer;
	}

	void Character::Initialize(int samplerate, int bufferSize, int modulationUpdateRate)
	{
		buffer = new float[bufferSize];
		bqBottom.SetSamplerate(samplerate);
		bqTop.SetSamplerate(samplerate);

		this->modulationUpdateRate = modulationUpdateRate;
		this->samplerate = samplerate;
	}

	void Character::SetParameter(CharacterParameters parameter, double value)
	{
		switch (parameter)
		{
		case CharacterParameters::Bottom:
			Bottom = (float)value;
			break;
		case CharacterParameters::Clip:
			Clip = (float)value;
			break;
		case CharacterParameters::Decimate:
			Decimate = (float)value;
			break;
		case CharacterParameters::Reduce:
			Reduce = (float)value;
			break;
		case CharacterParameters::Top:
			Top = (float)value;
			break;
		}
	}

	void Character::Process(float* input, int len)
	{
		if (!IsEnabled)
		{
			AudioLib::Utils::Copy(input, buffer, len);
			return;
		}

		Update();

		for (int i = 0; i < len; i++)
		{
			decimateCounter++;
			if (decimateCounter >= decimateFactor)
			{
				lastDecimateVal = input[i];
				decimateCounter -= decimateFactor;
			}

			float val = lastDecimateVal;

			if (reduceOn)
			{
				val = std::floorf(val * bitReduceFactor) * bitReduceFactorInv;
			}

			if (clipOn)
			{
				val = clip * 10 * val;
				val = Utils::Limit(val, -1, 1);
				//val = Utils::QuickNonlin(val);
			}

			buffer[i] = val;
		}

		bqBottom.Process(buffer, buffer, len);
		bqTop.Process(buffer, buffer, len);
	}

	float* Character::GetOutput()
	{
		return buffer;
	}

	void Character::SetGlide(float value)
	{
		if (value < 0.0001)
		{
			glideRate = 10000;
			return;
		}

		// formula: 2 ^ (-8*x) / (2^(-7))
		// goes from 32 oct/second to 0.5 oct/second
		float divisor = (float)std::pow(2, -7);
		float octavesPerSecond = (float)std::pow(2, -8 * value) / divisor;
		float notesPerSample = octavesPerSecond * 12.0f / samplerate;
		glideRate = notesPerSample;
	}

	void Character::Update()
	{
		// ----------- Set bottom and Top -------------

		// The current shelf biquads are somewhat expensive to update, also this doesn't really need audio rate modulation
		// update once every 4 update ticks.
		if (biquadUpdateCounter == 0)
		{
			float btm = Utils::Limit(Bottom + BottomMod, 0, 1);
			float top = Utils::Limit(Top + TopMod, 0, 1);

			if (currentPitch < Note)
			{
				currentPitch += glideRate * modulationUpdateRate;
				if (currentPitch > Note)
					currentPitch = (float)Note;
			}
			else if (currentPitch > Note)
			{
				currentPitch -= glideRate * modulationUpdateRate;
				if (currentPitch < Note)
					currentPitch = (float)Note;
			}

			bqBottom.Frequency = Utils::Note2Freq(currentPitch) * 1.5f;
			bqTop.Frequency = 8000.0f; // Utils::Note2Freq(currentPitch) * 10.0f;
			if (bqBottom.Frequency > 1000) bqTop.Frequency = 1000.0f;
			//if (bqTop.Frequency > 8000) bqTop.Frequency = 8000.0f;
			bqBottom.SetGain(1.0f + btm);
			bqTop.SetGain(1.0f + top);
			bqBottom.Update();
			bqTop.Update();
			biquadUpdateCounter = 4;
		}

		biquadUpdateCounter--;

		// ------------ Set decimate and reduce --------------

		float decimate = Utils::Limit(Decimate + DecimateMod, 0, 1);
		float reduce = Utils::Limit(Reduce + ReduceMod, 0, 1);
		reduceOn = Reduce > 0.0001;

		decimateFactor = (float)std::pow(2, decimate * 4);
		bitReduceFactor = (1.0f - reduce) * (1.0f - reduce) * 256.0f;
		if (bitReduceFactor < 2) bitReduceFactor = 2.0f;
		bitReduceFactorInv = 1.0f / bitReduceFactor;

		// ------------ Set Clip ---------------------

		clip = Utils::Limit(Clip + ClipMod, 0.0f, 0.9f) + 0.1f;
		clipOn = Clip > 0.0001;
	}
}

