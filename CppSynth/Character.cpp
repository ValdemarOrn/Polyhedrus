#include "Character.h"

namespace Leiftur
{
	Character::Character()
	{
		updateCounter = 0;
		buffer = 0;
	}

	Character::~Character()
	{
		delete buffer;
	}

	void Character::Initialize(int samplerate, int bufferSize, int modulationUpdateRate)
	{
		buffer = new float[bufferSize];

		this->modulationUpdateRate = modulationUpdateRate;
		this->samplerate = samplerate;
		Update();
	}

	void Character::SetParameter(CharacterParameters parameter, double value)
	{

	}

	void Character::Process(float* input, int len)
	{
		for (int i = 0; i < len; i++)
		{
			if (updateCounter <= 0)
			{
				Update();
				updateCounter = modulationUpdateRate;
			}

			buffer[i] = input[i];
			updateCounter--;
		}
	}

	float* Character::GetOutput()
	{
		return buffer;
	}

	void Character::Update()
	{

	}

}

