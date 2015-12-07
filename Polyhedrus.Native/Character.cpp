#include "Character.h"
#include "AudioLib/Utils.h"
#include "AudioLib/MathDefs.h"

using AudioLib::Utils;

namespace Polyhedrus
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
		buffer = new float[bufferSize]();
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

		register float temp = lastDecimateVal;
		for (int i = 0; i < len; i++)
		{
			decimateCounter++;
			if (decimateCounter >= decimateFactor)
			{
				temp = input[i];
				decimateCounter -= decimateFactor;
			}
			buffer[i] = temp;
		}

		lastDecimateVal = temp;

		if (reduceOn)
		{
			for (int i = 0; i < len; i++)
			{
				buffer[i] = std::floorf(buffer[i] * bitReduceFactor) * bitReduceFactorInv;
			}
		}

		if (clipOn)
		{
			for (int i = 0; i < len; i++)
			{
				buffer[i] = Utils::Limit(buffer[i] * clip, -1, 1);
			}
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

			bqBottom.Frequency = Utils::Note2Freq(currentPitch) * 2.0f;
			bqTop.Frequency = 8000.0f; // Utils::Note2Freq(currentPitch) * 10.0f;
			if (bqBottom.Frequency > 1000) bqBottom.Frequency = 1000.0f;
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
		float bits = (float)std::pow(2, (1 - reduce) * 4);

		reduceOn = Reduce > 0.0001;

		decimateFactor = (float)std::pow(2, decimate * 4);
		bitReduceFactor = (float)std::pow(2, bits - 1);
		bitReduceFactorInv = 1.0f / bitReduceFactor;

		// ------------ Set Clip ---------------------

		clip = Utils::Limit(Clip + ClipMod, 0.0f, 0.9f) * 10 + 1.0f;
		clipOn = Clip > 0.0001;
	}

	std::vector<uint8_t> Character::GetVisual(CharacterParameters parameter, int* baseLevel)
	{
		std::vector<float> floatOutput;
		std::vector<uint8_t> output;
		float min = 9999;
		float max = -9999;

		if (parameter == CharacterParameters::Bottom || parameter == CharacterParameters::Top)
		{
			*baseLevel = 0;
			min = 0;
			max = 30;

			for (int i = 0; i < 110; i++)
			{
				float f = 10.0f * (float)std::pow(2, i * 0.1);
				float response = bqBottom.GetResponse(f) * bqTop.GetResponse(f);
				float db = (float)AudioLib::Utils::Gain2DB(response);
				if (db < min) db = min;
				if (db > max) db = max;
				floatOutput.push_back(db);
			}
		}
		else if (parameter == CharacterParameters::Decimate)
		{
			*baseLevel = 128;

			float decimate = Utils::Limit(Decimate, 0, 1);
			float factor = (float)std::pow(2, decimate * 4);
			float value = 0.0;
			float lastMod = 999;

			for (int i = 0; i < 256; i++)
			{
				float newMod = (float)std::fmod(i, factor * 2);
				if (newMod < lastMod)
					value = (float)std::sin(i / 256.0 * 2 * M_PI);
				lastMod = newMod;

				if (value < min) min = value;
				if (value > max) max = value;
				floatOutput.push_back(value);
			}
		}
		else if (parameter == CharacterParameters::Reduce)
		{
			*baseLevel = 128;

			float reduce = Utils::Limit(Reduce, 0, 1);
			float bits = (float)std::pow(2, (1 - reduce) * 4);
			float factor = (float)std::pow(2, bits - 1);
			float factorInv = 1.0f / factor;

			for (int i = 0; i < 256; i++)
			{
				float val = (float)std::sin(i / 256.0 * 2 * M_PI);
				float value = ((int)(val * factor)) * factorInv;
				
				if (value < min) min = value;
				if (value > max) max = value;
				floatOutput.push_back(value);
			}
		}
		else if (parameter == CharacterParameters::Clip)
		{
			*baseLevel = 128;

			float factor = Utils::Limit(Clip, 0.0f, 0.9f) * 10 + 1.0f;

			for (int i = 0; i < 256; i++)
			{
				float value = (float)std::sin(i / 256.0 * 2 * M_PI) * factor;
				value = Utils::Limit(value, -1.0f, 1.0f);

				if (value < min) min = value;
				if (value > max) max = value;
				floatOutput.push_back(value);
			}
		}

		// convert floats to byte values
		float scaler = 1.0f / (max - min);
		for (int i = 0; i < (int)floatOutput.size(); i++)
		{
			float val = (floatOutput[i] - min) * scaler;
			output.push_back((uint8_t)(val * 255.99));
		}

		return output;
	}
}

