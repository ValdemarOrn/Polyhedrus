#ifndef POLYHEDRUS_CHARACTER
#define POLYHEDRUS_CHARACTER

#include "AudioLib/Biquad.h"
#include "Parameters.h"
#include "AudioLib/Biquad.h"


namespace Polyhedrus
{
	class Character
	{
	public:
		bool IsEnabled;
		int Note;

		float Bottom;
		float Top;
		float Decimate;
		float Reduce;
		float Clip;

		float BottomMod;
		float TopMod;
		float DecimateMod;
		float ReduceMod;
		float ClipMod;

	private:
		float* buffer;
		int samplerate;
		int modulationUpdateRate;
		AudioLib::Biquad bqBottom;
		AudioLib::Biquad bqTop;
		int biquadUpdateCounter;

		float decimateCounter;
		float lastDecimateVal;
		
		float glideRate;
		float currentPitch;

		float clip;
		float decimateFactor;
		float bitReduceFactor;
		float bitReduceFactorInv;
		bool reduceOn;
		bool clipOn;

	public:
		Character();
		~Character();
		void Initialize(int samplerate, int bufferSize, int modulationUpdateRate);
		void SetParameter(CharacterParameters parameter, double value);
		void Process(float* input, int len);
		float* GetOutput();
		void SetGlide(float value);
		void Update();
		std::vector<uint8_t> GetVisual(CharacterParameters parameter, int* baseLevel);
	};
}

#endif