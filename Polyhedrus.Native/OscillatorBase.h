#pragma once

#include <vector>
#include <memory>

namespace Polyhedrus
{
	enum class OscillatorType
	{
		Wavetable = 0,
	};

	class OscillatorBase
	{
	public:
		bool Keytrack;
		int Note;
		int Octave;
		int Semi;
		int Cent;
		float Linear;

		float Phase;
		float Shape;

		float PitchMod;
		float LinearMod;
		float ShapeMod;

		float SelfFm;
		float* FmBuffer;

		virtual ~OscillatorBase() { }
		virtual OscillatorType GetType() const = 0;
		virtual void Initialize(int samplerate, int bufferSize, int modulationUpdateRate) = 0;
		virtual void SetGlide(float value) = 0;
		virtual void Reset() = 0;
		virtual void Process(int count) = 0;
		virtual float* GetOutput() const = 0;
		virtual std::vector<uint8_t> GetVisual() const = 0;
		virtual std::string GetShapeString() const = 0;
	};
}
