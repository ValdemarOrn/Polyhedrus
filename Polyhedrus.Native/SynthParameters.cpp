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

		if (module == Module::Delay && parameter == (int)DelayParameters::DelayL)
			text = this->Delay.GetDelayLString();
		else if (module == Module::Delay && parameter == (int)DelayParameters::DelayR)
			text = this->Delay.GetDelayRString();
		else
			text = FormatParameter(module, parameter, value);

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
