#include "Synth.h"
#include "ParameterFormatters.h"

namespace Polyhedrus
{
	std::string Synth::FormatParameter(Module module, int parameter, double value)
	{
		auto info = Parameters::ParamInfo[module][parameter];
		return info.Formatter(value);
	}

	void Synth::SendBackParameter(Module module, int parameter)
	{
		auto idx = PackParameter(module, parameter);
		double value = currentPreset.Values[idx];
		std::string text;

		if (module == Module::Osc1 && parameter == (int)OscParameters::Shape)
		{
			text = Voices[0].osc1->GetShapeString();
		}
		else if (module == Module::Osc2 && parameter == (int)OscParameters::Shape)
		{
			text = Voices[0].osc2->GetShapeString();
		}
		else if (module == Module::Osc3 && parameter == (int)OscParameters::Shape)
		{
			text = Voices[0].osc3->GetShapeString();
		}
		else if (module == Module::Delay && parameter == (int)DelayParameters::DelayL)
		{
			text = this->Delay.GetDelayLString();
		}
		else if (module == Module::Delay && parameter == (int)DelayParameters::DelayR)
		{
			text = this->Delay.GetDelayRString();
		}
		else if (module == Module::FilterMain && parameter == (int)FilterMainParameters::Mode)
		{
			text = this->Voices[0].mainFilterL->GetModeString();
		}
		else
		{
			text = FormatParameter(module, parameter, value);
		}

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
