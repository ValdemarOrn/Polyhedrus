#include "Synth.h"
#include "Osc/OscMessage.h"
#include "FilterCascade.h"
#include "Lfo.h"
#include "Envelope.h"

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
		auto freq = FilterCascade::GetCvFreq((float)(value * 10));
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
		else if (module == Module::EnvAmp || module == Module::EnvFilter || module == Module::EnvMod)
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
			case EnvParameters::Exponent:
			case EnvParameters::Retrigger:
				return FormatOnOff(value);
			}
		}
		else if (module == Module::Lfo1 || module == Module::Lfo2)
		{
			switch ((LfoParameters)parameter)
			{
			case LfoParameters::Attack:
			case LfoParameters::Decay:
			case LfoParameters::Release:
				return FormatTime(Envelope::GetDecayTime(value));
			case LfoParameters::Freq:
				return FormatLfoFreq(value);
			case LfoParameters::Phase:
				return value >= 0.999 ? "Free" : FormatIntRounded(value * 360) + " Deg";
			case LfoParameters::Shape:
				return FormatIntFloor(value);
			case LfoParameters::Sustain:
				return FormatPercent(value);
			case LfoParameters::Sync:
				return FormatOnOff(value);
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

		return FormatDecimal3(value);
	}

	void Synth::SendBackParameter(Module module, int parameter)
	{
		auto idx = PackParameter(module, parameter);
		double value = parameters[idx];
		auto text = FormatParameter(module, parameter, value);
		formattedParameters[idx] = text;
		OscMessage oscMsg("/Control/ParameterResponse");
		oscMsg.SetInt((int)module);
		oscMsg.SetInt(parameter);
		oscMsg.SetFloat((float)value);
		oscMsg.SetString(text);
		auto bytes = oscMsg.GetBytes();
		udpTranceiver->Send(bytes);
	}
}
