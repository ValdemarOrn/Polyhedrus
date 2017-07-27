#pragma once

#include <vector>
#include <memory>
#include "AudioLib/Utils.h"
#include "Utils.h"
#include "AudioLib/ValueTables.h"

namespace Polyhedrus
{
	enum class OscillatorType
	{
		Wavetable = 0,
	};

	class OscillatorBase
	{
	public:
		float Keytrack;
		int Note;
		int Octave;
		int Semi;
		int Cent;
		float Linear;
		float BaseHz;
		float BaseNote;

		float Glide;
		float GlideFactor;

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
		virtual void UpdateGlide() = 0;
		virtual void Reset() = 0;
		virtual void Process(int count) = 0;
		virtual float* GetOutput() const = 0;
		virtual std::vector<uint8_t> GetVisual() const = 0;
		virtual std::string GetShapeString() const = 0;

		virtual std::string GetLinearString()
		{
			return SPrint("%0.2f", Linear) + " Hz";
		}

		virtual std::string GetGlideString()
		{
			return SPrint("%0.2f", GlideFactor) + "x";
		}

		virtual void SetBaseHz(double value)
		{
			BaseHz = GetLinearFrequency(value);
			BaseNote = AudioLib::Utils::Freq2NoteT(BaseHz);
		}

		virtual std::string GetBaseHzString()
		{
			return SPrint("%0.2f", BaseHz) + " Hz";
		}

		inline static float GetLinearFrequency(double value)
		{
			return (float)(AudioLib::ValueTables::Get(value, AudioLib::ValueTables::Response3Dec) * 10000);
		}

		
	};
}
