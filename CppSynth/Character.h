#ifndef LEIFTUR_CHARACTER
#define LEIFTUR_CHARACTER

#include "AudioLib/Biquad.h"
#include "Parameters.h"

namespace Leiftur
{
	class Character
	{
	public:
		bool IsEnabled;

	private:
		float* buffer;
		int samplerate;
		int modulationUpdateRate;
		int updateCounter;

	public:
		Character();
		~Character();
		void Initialize(int samplerate, int bufferSize, int modulationUpdateRate);
		void SetParameter(CharacterParameters parameter, double value);
		void Process(float* input, int len);
		float* GetOutput();
	private:
		void Update();
	};
}

#endif