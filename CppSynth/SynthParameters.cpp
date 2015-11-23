#include "Synth.h"
#include "ParameterFormatters.h"

namespace Leiftur
{
	std::string Synth::FormatParameter(Module module, int parameter, double value)
	{
		auto info = Parameters::ParamInfo[module][parameter];

		if (module == Module::Osc1 || module == Module::Osc2 || module == Module::Osc3 || module == Module::Mixer || module == Module::ModuleSwitches
			|| module == Module::Character || module == Module::FilterHp || module == Module::FilterMain || module == Module::Drive
			|| module == Module::EnvAmp || module == Module::EnvFilter || module == Module::Mod1 || module == Module::Mod2 || module == Module::Mod3
			|| module == Module::Arp)
		{
			return info.Formatter(value);
		}
		else if (module == Module::Voices)
		{
			switch ((VoiceParameters)parameter)
			{
			case VoiceParameters::Bend:
				return ParameterFormatters::FormatIntFloor(value);
			case VoiceParameters::Detune:
				return ParameterFormatters::FormatPercent(value);
			case VoiceParameters::Glide:
				return ParameterFormatters::FormatPercent(value);
			case VoiceParameters::Master:
				return ParameterFormatters::FormatDecimal3(value);
			case VoiceParameters::Polyphony:
				return ParameterFormatters::FormatIntFloor(value);
			case VoiceParameters::Spread:
				return ParameterFormatters::FormatPercent(value);
			case VoiceParameters::Unison:
				return ParameterFormatters::FormatIntFloor(value);
			case VoiceParameters::VoiceMode:
				return ParameterFormatters::FormatIntFloor(value);
			}
		}
		else if (module == Module::Chorus)
		{
			switch ((ChorusParameters)parameter)
			{
			case ChorusParameters::Enable1:
			case ChorusParameters::Enable2:
				return ParameterFormatters::FormatOnOff(value);
			case ChorusParameters::Rate1:
			case ChorusParameters::Rate2:
				return ParameterFormatters::FormatLfoFreq(value);
			case ChorusParameters::Depth1:
			case ChorusParameters::Depth2:
			case ChorusParameters::Width:
			case ChorusParameters::Quality:
			case ChorusParameters::Wet:
				return ParameterFormatters::FormatPercent(value);
			}
		}
		else if (module == Module::Delay)
		{
			switch ((DelayParameters)parameter)
			{
			case DelayParameters::DelayL:
			case DelayParameters::DelayR:
				return ParameterFormatters::FormatPercent(value);
			case DelayParameters::FeedbackL:
			case DelayParameters::FeedbackR:
				return ParameterFormatters::FormatPercent(value);
			case DelayParameters::Lowpass:
			case DelayParameters::Highpass:
			case DelayParameters::Saturate:
			case DelayParameters::DiffuseAmount:
			case DelayParameters::DiffuseSize:
			case DelayParameters::Wet:
				return ParameterFormatters::FormatPercent(value);
			case DelayParameters::Sync:
				return ParameterFormatters::FormatOnOff(value);
			}
		}
		else if (module == Module::Macros)
		{
			return ParameterFormatters::FormatDecimal2(value);
		}

		return ParameterFormatters::FormatDecimal3(value);
	}

	void Synth::SendBackParameter(Module module, int parameter)
	{
		auto idx = PackParameter(module, parameter);
		double value = currentPreset.Values[idx];
		auto text = FormatParameter(module, parameter, value);
		formattedParameters[idx] = text;
		OscMessage oscMsg("/Control/ParameterData");
		oscMsg.SetInt((int)module);
		oscMsg.SetInt(parameter);
		oscMsg.SetFloat((float)value);
		oscMsg.SetString(text);
		auto bytes = oscMsg.GetBytes();
		udpTranceiver->Send(bytes);
	}
}
