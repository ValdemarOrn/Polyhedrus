#pragma once

#include "Parameters.h"

namespace Polyhedrus
{
	enum class FilterMainType
	{
		TrueZero = 0,
		DualSvf = 1,
	};

	class FilterMain
	{
	public:
		bool IsEnabled;
		float Cutoff;
		float Resonance;
		float Drive;
		float Mode;

	protected:
		float CutoffMod = 0;
		float ResonanceMod = 0;
		float DriveMod = 0;
		float ModeMod = 0;

	public:
		virtual ~FilterMain() { }

		virtual FilterMainType GetType() const = 0;
		virtual void Initialize(int samplerate, int bufferSize, int modulationUpdateRate) = 0;
		virtual void SetParameter(FilterMainParameters parameter, double value) = 0;
		virtual void Process(float* input, int len) = 0;
		virtual float* GetOutput() = 0;
		virtual std::vector<uint8_t> GetVisual() = 0;
		virtual std::vector<uint8_t> GetDriveVisual() = 0;
		virtual std::string GetModeString() = 0;

		virtual void SetCutoffMod(float modAmount) { CutoffMod = modAmount; }
		virtual void SetResonanceMod(float modAmount) { ResonanceMod = modAmount; }
		virtual void SetDriveMod(float modAmount) { DriveMod = modAmount; }
		virtual void SetModeMod(float modAmount) { ModeMod = modAmount; }
	};
}

