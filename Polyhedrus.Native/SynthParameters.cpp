#include "Synth.h"
#include "ParameterFormatters.h"

namespace Polyhedrus
{
	std::string Synth::FormatParameter(Module module, int parameter, double value)
	{
		auto info = Parameters::ParamInfo[module][parameter];
		return info.Formatter(value);
	}

	std::string Synth::GetParameterText(Module module, int parameter, double value)
	{
		if (module == Module::Osc1)
		{
			if (parameter == (int)OscParameters::Shape)
				return Voices[0].osc1->GetShapeString();
			if (parameter == (int)OscParameters::BaseHz)
				return Voices[0].osc1->GetBaseHzString();
			if (parameter == (int)OscParameters::Linear)
				return Voices[0].osc1->GetLinearString();
			if (parameter == (int)OscParameters::GlideFactor)
				return Voices[0].osc1->GetGlideString();
		}
		else if (module == Module::Osc2)
		{
			if (parameter == (int)OscParameters::Shape)
				return Voices[0].osc2->GetShapeString();
			if (parameter == (int)OscParameters::BaseHz)
				return Voices[0].osc2->GetBaseHzString();
			if (parameter == (int)OscParameters::Linear)
				return Voices[0].osc2->GetLinearString();
			if (parameter == (int)OscParameters::GlideFactor)
				return Voices[0].osc2->GetGlideString();
		}
		else if (module == Module::Osc3)
		{
			if (parameter == (int)OscParameters::Shape)
				return Voices[0].osc3->GetShapeString();
			if (parameter == (int)OscParameters::BaseHz)
				return Voices[0].osc3->GetBaseHzString();
			if (parameter == (int)OscParameters::Linear)
				return Voices[0].osc3->GetLinearString();
			if (parameter == (int)OscParameters::GlideFactor)
				return Voices[0].osc3->GetGlideString();
		}
		else if (module == Module::Delay)
		{
			if (parameter == (int)DelayParameters::DelayL)
				return this->Delay.GetDelayLString();
			if (parameter == (int)DelayParameters::DelayR)
				return this->Delay.GetDelayRString();
		}
		else if (module == Module::FilterMain)
		{
			if (parameter == (int)FilterMainParameters::Mode)
				return this->Voices[0].mainFilterL->GetModeString();
		}
		
		return FormatParameter(module, parameter, value);
	}

	void Synth::SendBackParameter(Module module, int parameter)
	{
		auto idx = PackParameter(module, parameter);
		double value = currentPreset.Values[idx];
		std::string text = GetParameterText(module, parameter, value);

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
