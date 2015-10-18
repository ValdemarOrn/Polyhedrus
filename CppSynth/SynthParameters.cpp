/*#include "Synth.h"

namespace Leiftur
{
	double Synth::TranslateValue(Module module, int parameter, double value)
	{
		if (module == Module::Osc1 || module == Module::Osc2 || module == Module::Osc3)
		{
			auto para = (OscParameters)parameter;
			switch (para)
			{
			case OscParameters::Cent:
				return value * 100 - 50;
			case OscParameters::Octave:
				return (int)(-2 + 4.999 * value);
			case OscParameters::Pan:
				return -2 + 2 * value;
			case OscParameters::Semi:
				return (int)(-12 + 24.999 * value);
			default:
				return value;
			}
		}
		else if (module == Module::Mixer)
		{
			auto para = (MixerParameters)parameter;
			switch (para)
			{
			case MixerParameters::Color:
				return (int)(value * 2.999);
			default:
				return value;
			}
		}
		else if (module == Module::FilterHp)
		{
			auto para = (FilterHpParameters)parameter;
			switch (para)
			{
			default:
				return value;
			}
		}
		else if (module == Module::FilterMain)
		{
			auto para = (FilterMainParameters)parameter;
			switch (para)
			{
			case FilterMainParameters::Type:
				return (int)(value * 0.999);
			default:
				return value;
			}
		}
		else if (module == Module::EnvAmp || module == Module::EnvFilter || module == Module::EnvMod)
		{
			auto para = (EnvParameters)parameter;
			switch (para)
			{
			default:
				return value;
			}
		}
	}
}
*/