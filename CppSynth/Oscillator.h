#ifndef LEIFTUR_OSCILLATOR
#define LEIFTUR_OSCILLATOR

#include "Default.h"
#include "WavetableManager.h"
//#include <memory>

namespace Leiftur
{
	struct Oscillator
	{
	public:
		int Note;
		int Octave;
		int Semi;
		int Cent;
		float PitchMod;

		float Phase;
		float Shape;
		float ShapeMod;

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

	public:
		Oscillator();
		~Oscillator();
		void Initialize(int samplerate, int bufferSize, int modulationUpdateRate);
		void SetWavetable(std::shared_ptr<Wavetable> wavetable);
		void Reset();
		void Process(int count);
		float* GetOutput();

	private:
		void Update();
	};
}

#endif
