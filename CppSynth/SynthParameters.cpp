#include "Synth.h"
#include "Osc/OscMessage.h"

namespace Leiftur
{
	std::string sprintString(const char* formatter, double values)
	{
		char buffer[256];
		sprintf(buffer, formatter, values);
		return std::string(buffer);
	}

	std::string Synth::FormatParameter(Module module, int parameter, double value)
	{
		switch (PackParameter(module, parameter))
		{
		case PackParameter(Module::Osc1, (int)OscParameters::Pan):
		case PackParameter(Module::Osc2, (int)OscParameters::Pan):
		case PackParameter(Module::Osc3, (int)OscParameters::Pan):
			return sprintString("%.0f", value * 100);
		default:
			return sprintString("%.4f", value);
		}
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
		oscMsg.SetFloat(value);
		oscMsg.SetString(text);
		auto bytes = oscMsg.GetBytes();
		udpTranceiver->Send(bytes);
	}
}
