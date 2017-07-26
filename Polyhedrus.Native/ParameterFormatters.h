#ifndef POLYHEDRUS_PARAMETER_FORMATTERS
#define POLYHEDRUS_PARAMETER_FORMATTERS

#include <string>

#include "Utils.h"
#include "Osc/OscMessage.h"
#include "FilterCascade.h"
#include "Modulator.h"
#include "Envelope.h"
#include "MixerSettings.h"
#include "NoiseSimple.h"
#include "Arpeggiator.h"
#include "AudioLib/TempoSync.h"

namespace Polyhedrus
{
	class ParameterFormatters
	{
	public:

		static inline std::string FormatPercent(double value)
		{
			return SPrint("%.0f", value * 100) + " %";
		}

		static inline std::string FormatPercentPrecise(double value)
		{
			return SPrint("%.2f", value * 100) + "%";
		}

		static inline std::string FormatDecimal1(double value)
		{
			return SPrint("%.1f", value);
		}

		static inline std::string FormatDecimal2(double value)
		{
			return SPrint("%.2f", value);
		}

		static inline std::string FormatDecimal3(double value)
		{
			return SPrint("%.3f", value);
		}

		static inline std::string FormatRouting(double value)
		{
			if (RoutingStage::Character == (RoutingStage)Parameters::FloorToInt(value))
				return "Character";
			if (RoutingStage::Direct == (RoutingStage)Parameters::FloorToInt(value))
				return "Direct";
			if (RoutingStage::HpFilter == (RoutingStage)Parameters::FloorToInt(value))
				return "HP Filter";
			if (RoutingStage::MainFilter == (RoutingStage)Parameters::FloorToInt(value))
				return "Main Filter";
			else
				return "---";
		}

		static inline std::string FormatPhase(double value)
		{
			return value >= 0.999 ? "Free" : ParameterFormatters::FormatIntRounded(value * 360) + " Deg";
		}

		static inline std::string FormatNoiseType(double value)
		{
			int val = Parameters::FloorToInt(value);
			if (val == (int)NoiseType::White)
				return "White";
			if (val == (int)NoiseType::Pink)
				return "Pink";
			if (val == (int)NoiseType::Brown)
				return "Brown";

			return "";
		}

		static inline std::string FormatIntFloor(double value)
		{
			return SPrint("%0.0f", Parameters::FloorToInt(value));
		}

		static inline std::string FormatIntRounded(double value)
		{
			return SPrint("%0.0f", Parameters::RoundToInt(value));
		}

		static inline std::string FormatExp2(double value, double scale)
		{
			auto val = std::pow(2, value * scale);
			return FormatDecimal2(val);
		}

		static inline std::string FormatFilterCutoff(double value)
		{
			auto freq = FilterCascade::GetCvFreq((float)value);
			return FormatIntRounded(freq) + " Hz";
		}

		static inline std::string FormatLfoFreq(double value)
		{
			auto freq = Modulator::GetFrequency((float)value);
			return FormatDecimal2(freq) + " Hz";
		}

		static inline std::string FormatEnvTime(double value)
		{
			return FormatDecimal2(Envelope::GetDecayTime((float)value)) + " Sec";
		}

		static inline std::string FormatTime(double value)
		{
			return FormatDecimal2(value) + " Sec";
		}

		static inline std::string FormatOnOff(double value)
		{
			return std::string(value < 0.5 ? "Off" : "On");
		}

		static inline std::string FormatModulatorSteps(double value)
		{
			int stepCount = (int)(ValueTables::Get(value, ValueTables::Response3Oct) * 256);
			if (stepCount >= 256)
				return std::string("Smooth");
			else
				return FormatIntRounded(stepCount) + " Steps";
		}

		static inline std::string FormatTempoSync(double value)
		{
			AudioLib::Quantization noteQuant = (AudioLib::Quantization)(int)(AudioLib::Utils::Limit((float)value, 0.0f, 1.0f) * ((int)AudioLib::Quantization::_1d + 0.999f));
			return AudioLib::TempoSync::ToString(noteQuant);
		}

		static inline std::string FormatArpPattern(double value)
		{
			auto ptn = (ArpPattern)Parameters::FloorToInt(value);
			switch (ptn)
			{
			case ArpPattern::Up:
				return std::string("Up");
			case ArpPattern::Down:
				return std::string("Down");
			case ArpPattern::UpDown1:
				return std::string("Up Down");
			case ArpPattern::UpDown2:
				return std::string("Up Down Repeat");
			case ArpPattern::DownUp1:
				return std::string("Down Up");
			case ArpPattern::DownUp2:
				return std::string("Down Up Repeat");
			} 

			return std::string("???");
		}

		static inline std::string FormatFilterType(double value)
		{
			int type = Parameters::FloorToInt(value);
			switch (type)
			{
			case 0:
				return std::string("TrueZero");
			case 1:
				return std::string("State-Variable VA");
			}

			return std::string("???");
		}
	};
}

#endif
