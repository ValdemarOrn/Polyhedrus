#pragma once

#include "Parameters.h"

namespace Polyhedrus
{
	enum class FilterMainType
	{
		TrueZero = 0,
		DualSvf = 1,
	};

	class FilterMainXX
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
		virtual ~FilterMainXX() { }

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
	/*
	class FilterMain
	{
	private:
		int samplerate;
		int modulationUpdateRate;
		FilterCascade cascadeFilter;
		FilterTrueZero trueZeroFilter;
		FilterCascadeZero cascadeZeroFilter;
		FilterDualSvf svfFilter;
		AudioLib::Hp1 hp;
		float* bypassBuffer;
		FilterMainType type;

	public:
		bool IsEnabled;
		float cutoff;
		float resonance;
		float drive;

		FilterMain();
		~FilterMain();
		void Initialize(int samplerate, int bufferSize, int modulationUpdateRate);
		void SetParameter(FilterMainParameters parameter, double value);
		void Process(float* input, int len);
		float* GetOutput();
		std::vector<uint8_t> GetVisual();
		std::vector<uint8_t> GetDriveVisual();
		inline std::string GetModeString()
		{
			if (type == FilterMainType::DualSvf)
			{
				float mode = svfFilter.Mode * 2;
				if (mode <= 1)
				{
					return SPrint("%02.0f", (1 - mode) * 100) + "% LP - " + SPrint("%02.0f", mode * 100) + "% BP";
				}
				else
				{
					mode = mode - 1;
					return SPrint("%02.0f", (1 - mode) * 100) + "% BP - " + SPrint("%02.0f", mode * 100) + "% HP";
				}
			}
			else
			{
				return std::string("000");
			}
		}

		inline void SetCutoffMod(float modAmount)
		{
			cascadeFilter.CutoffMod = modAmount;
			cascadeZeroFilter.CutoffMod = modAmount;
			svfFilter.CutoffMod = modAmount;
			trueZeroFilter.CutoffMod = modAmount;
		}

		inline void SetDriveMod(float modAmount)
		{
			cascadeFilter.DriveMod = modAmount;
			cascadeZeroFilter.DriveMod = modAmount;
			svfFilter.DriveMod = modAmount;
			trueZeroFilter.DriveMod = modAmount;
		}

		inline void SetResonanceMod(float modAmount)
		{
			cascadeFilter.ResonanceMod = modAmount;
			cascadeZeroFilter.ResonanceMod = modAmount;
			svfFilter.ResonanceMod = modAmount;
			trueZeroFilter.ResonanceMod = modAmount;
		}
	};*/
}

