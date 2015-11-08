#include "Character.h"
#include "AudioLib/Utils.h"

using namespace AudioLib::Utils;

namespace Leiftur
{
	Character::Character()
		: bqBottom(AudioLib::Biquad::FilterType::LowShelf, 48000)
		, bqTop(AudioLib::Biquad::FilterType::HighShelf, 48000)
	{
		Note = 60;
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
			Bottom = value;
			break;
		case CharacterParameters::Clip:
			Clip = value;
			break;
		case CharacterParameters::Decimate:
			Decimate = value;
			break;
		case CharacterParameters::Reduce:
			Reduce = value;
			break;
		case CharacterParameters::Top:
			Top = value;
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
				val = ((int)(val * bitReduceFactor)) * bitReduceFactorInv;
			}

			if (clipOn)
			{
				val = clip * 10 * val;
				val = Limit(val, -1, 1);
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
		float divisor = std::pow(2, -7);
		float octavesPerSecond = std::pow(2, -8 * value) / divisor;
		float notesPerSample = octavesPerSecond * 12.0 / samplerate;
		glideRate = notesPerSample;
	}

	void Character::Update()
	{
		// ----------- Set bottom and Top -------------

		float btm = Limit(Bottom + BottomMod, 0, 1);
		float top = Limit(Top + TopMod, 0, 1);

		if (currentPitch < Note)
		{
			currentPitch += glideRate * modulationUpdateRate;
			if (currentPitch > Note)
				currentPitch = Note;
		}
		else if (currentPitch > Note)
		{
			currentPitch -= glideRate * modulationUpdateRate;
			if (currentPitch < Note)
				currentPitch = Note;
		}

		bqBottom.Frequency = AudioLib::Utils::Note2Freq(currentPitch) * 1.5;
		bqTop.Frequency = AudioLib::Utils::Note2Freq(currentPitch) * 10;
		if (bqTop.Frequency > 8000) bqTop.Frequency = 8000;
		bqBottom.SetGain(1 + btm);
		bqTop.SetGain(1 + top);
		bqBottom.Update();
		bqTop.Update();

		// ------------ Set decimate and reduce --------------

		float decimate = Limit(Decimate + DecimateMod, 0, 1);
		float reduce = Limit(Reduce + ReduceMod, 0, 1);
		reduceOn = Reduce > 0.0001;

		decimateFactor = std::pow(2, decimate * 4);
		bitReduceFactor = (1 - reduce) * (1 - reduce) * 256;
		if (bitReduceFactor < 2) bitReduceFactor = 2;
		bitReduceFactorInv = 1.0 / bitReduceFactor;

		// ------------ Set Clip ---------------------

		clip = Limit(Clip + ClipMod, 0, 0.9) + 0.1;
		clipOn = Clip > 0.0001;
	}
}

