#ifndef LEIFTUR_PARAMETER_FORMATTERS
#define LEIFTUR_PARAMETER_FORMATTERS

#include <string>

#include "Utils.h"
#include "Osc/OscMessage.h"
#include "FilterCascade.h"
#include "Modulator.h"
#include "Envelope.h"
#include "MixerSettings.h"

namespace Leiftur
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
			auto freq = FilterCascade::GetCvFreq((float)(value * 10.3));
			return FormatIntRounded(freq) + " Hz";
		}

		static inline std::string FormatLfoFreq(double value)
		{
			auto freq = Modulator::GetFrequency((float)value);
			return FormatDecimal2(freq) + " Hz";
		}

		static inline std::string FormatTime(double value)
		{
			return FormatDecimal2(value) + " Sec";
		}

		static inline std::string FormatOnOff(double value)
		{
			return std::string(value < 0.5 ? "Off" : "On");
		}
	};
}

#endif
