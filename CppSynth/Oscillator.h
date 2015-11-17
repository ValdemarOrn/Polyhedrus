#ifndef LEIFTUR_OSCILLATOR
#define LEIFTUR_OSCILLATOR

#include "Default.h"
#include "WavetableManager.h"

namespace Leiftur
{
	struct Oscillator
	{
	public:
		bool Keytrack;
		int Note;
		int Octave;
		int Semi;
		int Cent;
		float Linear;

		float PitchMod;
		float LinearMod;

		float Phase;
		float Shape;
		float ShapeMod;
		
		float* FmBuffer;

	private:
		float* buffer;
		std::shared_ptr<Wavetable> wavetable;
		int samplerate;
		int modulationUpdateRate;
		uint32_t increment;
		uint32_t iterator;
		int updateCounter;
		int shiftValue;
		float waveMix;
		float* waveA;
		float* waveB;

		float glideRate;
		float currentPitch;

	public:
		Oscillator();
		~Oscillator();
		void Initialize(int samplerate, int bufferSize, int modulationUpdateRate);
		void SetWavetable(std::shared_ptr<Wavetable> wavetable);
		void SetGlide(float value);
		void Reset();
		void Process(int count);
		float* GetOutput();

	private:
		void Update();
	};
}

#endif
