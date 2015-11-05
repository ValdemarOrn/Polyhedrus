#include "Synth.h"
#include "Osc/OscMessage.h"
#include "FilterCascade.h"
#include "Lfo.h"
#include "Envelope.h"
#include "MixerSettings.h"

namespace Leiftur
{
	std::string SPrint(const char* formatter, double value)
	{
		char buffer[256];
		sprintf(buffer, formatter, value);
		return std::string(buffer);
	}

	std::string FormatPercent(double value)
	{
		return SPrint("%.0f", value * 100) + " %";
	}

	std::string FormatPercentPrecise(double value)
	{
		return SPrint("%.2f", value * 100) + "%";
	}

	std::string FormatDecimal1(double value)
	{
		return SPrint("%.1f", value);
	}

	std::string FormatDecimal2(double value)
	{
		return SPrint("%.2f", value);
	}

	std::string FormatDecimal3(double value)
	{
		return SPrint("%.3f", value);
	}

	std::string FormatIntFloor(double value)
	{
		return SPrint("%0.0f", Parameters::FloorToInt(value));
	}

	std::string FormatIntRounded(double value)
	{
		return SPrint("%0.0f", Parameters::RoundToInt(value));
	}

	std::string FormatExp2(double value, double scale)
	{
		auto val = std::pow(2, value * scale);
		return FormatDecimal2(val);
	}

	std::string FormatFilterCutoff(double value)
	{
		auto freq = FilterCascade::GetCvFreq((float)(value * 10.3));
		return FormatIntRounded(freq) + " Hz";
	}

	std::string FormatLfoFreq(double value)
	{
		auto freq = Lfo::GetFrequency((float)value);
		return FormatDecimal2(freq) + " Hz";
	}

	std::string FormatTime(double value)
	{
		return FormatDecimal2(value) + " Sec";
	}

	std::string FormatOnOff(double value)
	{
		return std::string(value < 0.5 ? "Off" : "On");
	}

	std::string Synth::FormatParameter(Module module, int parameter, double value)
	{
		if (module == Module::Osc1 || module == Module::Osc2 || module == Module::Osc3)
		{
			switch ((OscParameters)parameter)
			{
			case OscParameters::Pan:
				return FormatPercent(value);
			case OscParameters::Octave:
			case OscParameters::Semi:
			case OscParameters::Cent:
				return FormatIntFloor(value);
			case OscParameters::Shape:
				return FormatPercentPrecise(value);
			case OscParameters::Phase:
				return value >= 0.999 ? "Free" : FormatIntRounded(value * 360) + " Deg";
			case OscParameters::Waveform:
				return FormatIntFloor(value);
			case OscParameters::Routing:
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
		}
		else if (module == Module::Mixer)
		{
			switch ((MixerParameters)parameter)
			{
			case MixerParameters::Color:
				return FormatIntFloor(value);
			}
		}
		else if (module == Module::Character)
		{
			switch ((CharacterParameters)parameter)
			{
			case CharacterParameters::Decimate:
				return FormatExp2(value, 4);
			case CharacterParameters::Reduce:
				return FormatExp2(1.0 - value, 4);
			case CharacterParameters::Bottom:
			case CharacterParameters::Top:
			case CharacterParameters::Clip:
				return FormatPercent(value);
			}
		}
		else if (module == Module::FilterHp)
		{
			switch ((FilterHpParameters)parameter)
			{
			case FilterHpParameters::Cutoff:
				return FormatFilterCutoff(value);
			case FilterHpParameters::Resonance:
			case FilterHpParameters::Keytrack:
			case FilterHpParameters::Env:
				return FormatPercent(value);
			}
		}
		else if (module == Module::FilterMain)
		{
			switch ((FilterMainParameters)parameter)
			{
			case FilterMainParameters::Cutoff:
				return FormatFilterCutoff(value);
			case FilterMainParameters::Drive:
			case FilterMainParameters::Resonance:
			case FilterMainParameters::Keytrack:
			case FilterMainParameters::Env:
				return FormatPercent(value);
			case FilterMainParameters::Type:
				return FormatIntFloor(value);
			}
		}
		else if (module == Module::Drive)
		{
			switch ((DriveParameters)parameter)
			{
			case DriveParameters::Gain:
				return FormatPercent(value);
			case DriveParameters::Bias:
				return FormatDecimal2(value);
			case DriveParameters::Post:
				return value >= 0.5 ? "Post" : "Pre";
			case DriveParameters::Type:
				return FormatIntFloor(value);
			case DriveParameters::Mellow:
				return FormatPercent(value);
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
				return FormatTime(Envelope::GetDecayTime(value));
			case EnvParameters::Sustain:
			case EnvParameters::Velocity:
				return FormatPercent(value);
			case EnvParameters::Retrigger:
				return FormatOnOff(value);
			case EnvParameters::AttackCurve:
			case EnvParameters::DecayCurve:
			case EnvParameters::ReleaseCurve:
				return FormatDecimal2(value);
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
				return FormatTime(Envelope::GetDecayTime(value));
			case ModParameters::Freq:
				return FormatLfoFreq(value);
			case ModParameters::Phase:
				return value >= 0.999 ? "Free" : FormatIntRounded(value * 360) + " Deg";
			case ModParameters::Shape:
				return FormatIntFloor(value);
			case ModParameters::Sustain:
			case ModParameters::Slew:
			case ModParameters::Steps:
				return FormatPercent(value);
			case ModParameters::Sync:
				return FormatOnOff(value);
			case ModParameters::AttackCurve:
			case ModParameters::DecayCurve:
			case ModParameters::ReleaseCurve:
				return FormatDecimal2(value);
			}
		}
		else if (module == Module::Arp)
		{
			switch ((ArpParameters)parameter)
			{
			case ArpParameters::Bpm:
				return FormatIntRounded(value);
			case ArpParameters::Divide:
				return FormatIntFloor(value * 16);
			case ArpParameters::Gate:
				return FormatPercent(value);
			case ArpParameters::NotePtn:
				return FormatIntFloor(value);
			case ArpParameters::OctavePtn:
				return FormatIntFloor(value);
			case ArpParameters::Range:
				return FormatIntFloor(value);
			case ArpParameters::Sync:
				return FormatOnOff(value);
			}
		}
		else if (module == Module::Voices)
		{
			switch ((VoiceParameters)parameter)
			{
			case VoiceParameters::Bend:
				return FormatIntFloor(value);
			case VoiceParameters::Detune:
				return FormatPercent(value);
			case VoiceParameters::Glide:
				return FormatPercent(value);
			case VoiceParameters::Master:
				return FormatDecimal3(value);
			case VoiceParameters::Polyphony:
				return FormatIntFloor(value);
			case VoiceParameters::Spread:
				return FormatPercent(value);
			case VoiceParameters::Unison:
				return FormatIntFloor(value);
			case VoiceParameters::VoiceMode:
				return FormatIntFloor(value);
			}
		}
		else if (module == Module::Chorus)
		{
			switch ((ChorusParameters)parameter)
			{
			case ChorusParameters::Enable1:
			case ChorusParameters::Enable2:
				return FormatOnOff(value);
			case ChorusParameters::Rate1:
			case ChorusParameters::Rate2:
				return FormatLfoFreq(value);
			case ChorusParameters::Depth1:
			case ChorusParameters::Depth2:
			case ChorusParameters::Width:
			case ChorusParameters::Quality:
			case ChorusParameters::Wet:
				return FormatPercent(value);
			}
		}
		else if (module == Module::Delay)
		{
			switch ((DelayParameters)parameter)
			{
			case DelayParameters::DelayL:
			case DelayParameters::DelayR:
				return FormatPercent(value);
			case DelayParameters::FeedbackL:
			case DelayParameters::FeedbackR:
				return FormatPercent(value);
			case DelayParameters::Lowpass:
			case DelayParameters::Highpass:
			case DelayParameters::Saturate:
			case DelayParameters::Crossfeed:
			case DelayParameters::Diffuse:
			case DelayParameters::Wet:
				return FormatPercent(value);
			case DelayParameters::Sync:
				return FormatOnOff(value);
			}
		}
		else if (module == Module::Macros)
		{
			return FormatDecimal2(value);
		}

		return FormatDecimal3(value);
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
