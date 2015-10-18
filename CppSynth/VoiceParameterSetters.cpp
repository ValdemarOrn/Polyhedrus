#include "Voice.h"

namespace Leiftur
{
	void Voice::SetOscParameter(Module module, OscParameters parameter, double value)
	{
		auto update = [=](Oscillator* osc)
		{
			switch (parameter)
			{
			case OscParameters::Cent:
				osc->Cent = value * 100 - 50;
			case OscParameters::Octave:
				osc->Octave = (int)(-2 + 4.999 * value);
			case OscParameters::Semi:
				osc->Semi = (int)(-12 + 24.999 * value);
			case OscParameters::Phase:
				osc->Phase = value;
			case OscParameters::Shape:
				osc->Shape = value;
			case OscParameters::Waveform:
				osc->SetWaveform((int)(value * 1.999));
			}
		};

		if (module == Module::Osc1)
		{
			if (parameter == OscParameters::Pan)
				mixer.Osc1Pan = -1.0 + 2 * value;
			else if (parameter == OscParameters::Volume)
				mixer.Osc1Volume = value;
			else if (parameter == OscParameters::Slop)
			{ 
				// set Slop in mod matrix
			}
			else
				update(&osc1);
		}
		else if (module == Module::Osc2)
		{
			if (parameter == OscParameters::Pan)
				mixer.Osc2Pan = -1.0 + 2 * value;
			else if (parameter == OscParameters::Volume)
				mixer.Osc2Volume = value;
			else if (parameter == OscParameters::Slop)
			{
				// set Slop in mod matrix
			}
			else
				update(&osc2);
		}
		else if (module == Module::Osc3)
		{
			if (parameter == OscParameters::Pan)
				mixer.Osc3Pan = -1.0 + 2 * value;
			else if (parameter == OscParameters::Volume)
				mixer.Osc3Volume = value;
			else if (parameter == OscParameters::Slop)
			{
				// set Slop in mod matrix
			}
			else
				update(&osc3);
		}
	}

	void Voice::SetMixerParameter(Module module, MixerParameters parameter, double value)
	{
		switch (parameter)
		{
		case MixerParameters::Am12:
			mixer.Am12 = value;
		case MixerParameters::Am23:
			mixer.Am23 = value;
		case MixerParameters::Color:
			mixer.Color = (int)(value * 2.999);
		case MixerParameters::Fm12:
			mixer.Fm12 = value;
		case MixerParameters::Fm13:
			mixer.Fm13 = value;
		case MixerParameters::Fm23:
			mixer.Fm23 = value;
		case MixerParameters::Noise:
			mixer.Noise = value;
		}
	}

	void Voice::SetFilterHpParameter(Module module, FilterHpParameters parameter, double value)
	{
		if (parameter == FilterHpParameters::Env)
			modMatrix.FixedRoutes[ModMatrix::FixedRouteFilterHpEnv].Amount = value;
		else if (parameter == FilterHpParameters::Keytrack)
			modMatrix.FixedRoutes[ModMatrix::FixedRouteFilterHpKeytrack].Amount = value;
		else
			hpFilter.SetParameter(parameter, value);
	}

	void Voice::SetFilterMainParameter(Module module, FilterMainParameters parameter, double value)
	{
		if (parameter == FilterMainParameters::Env)
			modMatrix.FixedRoutes[ModMatrix::FixedRouteFilterMainEnv].Amount = value;
		else if (parameter == FilterMainParameters::Keytrack)
			modMatrix.FixedRoutes[ModMatrix::FixedRouteFilterMainKeytrack].Amount = value;
		else
			mainFilter.SetParameter(parameter, value);
	}

	void Voice::SetEnvParameter(Module module, EnvParameters parameter, double value)
	{
		if (module == Module::EnvAmp)
			ampEnv.SetParameter(parameter, value);
		else if (module == Module::EnvFilter)
			filterEnv.SetParameter(parameter, value);
		else if (module == Module::EnvMod)
			modEnv.SetParameter(parameter, value);
	}
}
