#include "Synth.h"
#include "ParameterFormatters.h"

namespace Leiftur
{
	std::string Synth::FormatParameter(Module module, int parameter, double value)
	{
		auto info = Parameters::ParamInfo[module][parameter];

		if (module == Module::Osc1 || module == Module::Osc2 || module == Module::Osc3 || module == Module::Mixer || module == Module::ModuleSwitches)
		{
			return info.Formatter(value);
		}
		else if (module == Module::Character)
		{
			switch ((CharacterParameters)parameter)
			{
			case CharacterParameters::Decimate:
				return ParameterFormatters::FormatExp2(value, 4);
			case CharacterParameters::Reduce:
				return ParameterFormatters::FormatExp2(1.0 - value, 4);
			case CharacterParameters::Bottom:
			case CharacterParameters::Top:
			case CharacterParameters::Clip:
				return ParameterFormatters::FormatPercent(value);
			}
		}
		else if (module == Module::FilterHp)
		{
			switch ((FilterHpParameters)parameter)
			{
			case FilterHpParameters::Cutoff:
				return ParameterFormatters::FormatFilterCutoff(value);
			case FilterHpParameters::Resonance:
			case FilterHpParameters::Keytrack:
			case FilterHpParameters::Env:
				return ParameterFormatters::FormatPercent(value);
			}
		}
		else if (module == Module::FilterMain)
		{
			switch ((FilterMainParameters)parameter)
			{
			case FilterMainParameters::Cutoff:
				return ParameterFormatters::FormatFilterCutoff(value);
			case FilterMainParameters::Drive:
			case FilterMainParameters::Resonance:
			case FilterMainParameters::Keytrack:
			case FilterMainParameters::Env:
				return ParameterFormatters::FormatPercent(value);
			case FilterMainParameters::Type:
				return ParameterFormatters::FormatIntFloor(value);
			}
		}
		else if (module == Module::Drive)
		{
			switch ((DriveParameters)parameter)
			{
			case DriveParameters::Gain:
				return ParameterFormatters::FormatPercent(value);
			case DriveParameters::Bias:
				return ParameterFormatters::FormatDecimal2(value);
			case DriveParameters::Post:
				return value >= 0.5 ? "Post" : "Pre";
			case DriveParameters::Type:
				return ParameterFormatters::FormatIntFloor(value);
			case DriveParameters::Mellow:
				return ParameterFormatters::FormatPercent(value);
			}
		}
		else if (module == Module::EnvAmp || module == Module::EnvFilter)
		{
			switch ((EnvParameters)parameter)
			{
			case EnvParameters::Attack:
			case EnvParameters::Hold:
			case EnvParameters::Decay:
			case EnvParameters::Release:
				return ParameterFormatters::FormatTime(Envelope::GetDecayTime((float)value));
			case EnvParameters::Sustain:
			case EnvParameters::Velocity:
				return ParameterFormatters::FormatPercent(value);
			case EnvParameters::Retrigger:
				return ParameterFormatters::FormatOnOff(value);
			case EnvParameters::AttackCurve:
			case EnvParameters::DecayCurve:
			case EnvParameters::ReleaseCurve:
				return ParameterFormatters::FormatDecimal2(value);
			}
		}
		else if (module == Module::Mod1 || module == Module::Mod2 || module == Module::Mod3)
		{
			switch ((ModParameters)parameter)
			{
			case ModParameters::Attack:
			case ModParameters::Hold:
			case ModParameters::Decay:
			case ModParameters::Release:
				return ParameterFormatters::FormatTime(Envelope::GetDecayTime((float)value));
			case ModParameters::Freq:
				return ParameterFormatters::FormatLfoFreq(value);
			case ModParameters::Phase:
				return value >= 0.999 ? "Free" : ParameterFormatters::FormatIntRounded(value * 360) + " Deg";
			case ModParameters::Shape:
				return ParameterFormatters::FormatIntFloor(value);
			case ModParameters::Sustain:
			case ModParameters::Slew:
			case ModParameters::Steps:
				return ParameterFormatters::FormatPercent(value);
			case ModParameters::Sync:
				return ParameterFormatters::FormatOnOff(value);
			case ModParameters::AttackCurve:
			case ModParameters::DecayCurve:
			case ModParameters::ReleaseCurve:
				return ParameterFormatters::FormatDecimal2(value);
			}
		}
		else if (module == Module::Arp)
		{
			switch ((ArpParameters)parameter)
			{
			case ArpParameters::Bpm:
				return ParameterFormatters::FormatIntRounded(value);
			case ArpParameters::Divide:
				return ParameterFormatters::FormatIntFloor(value * 16);
			case ArpParameters::Gate:
				return ParameterFormatters::FormatPercent(value);
			case ArpParameters::NotePtn:
				return ParameterFormatters::FormatIntFloor(value);
			case ArpParameters::OctavePtn:
				return ParameterFormatters::FormatIntFloor(value);
			case ArpParameters::Range:
				return ParameterFormatters::FormatIntFloor(value);
			case ArpParameters::Sync:
				return ParameterFormatters::FormatOnOff(value);
			}
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
