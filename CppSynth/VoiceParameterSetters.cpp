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
				break;
			case OscParameters::Octave:
				osc->Octave = (int)(4.999 * value) - 2;
				break;
			case OscParameters::Semi:
				osc->Semi = (int)(24.999 * value) - 12;
				break;
			case OscParameters::Phase:
				osc->Phase = value;
				break;
			case OscParameters::Shape:
				osc->Shape = value;
				break;
			case OscParameters::Waveform:
				osc->SetWaveform((int)(value * 1.999));
				break;
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
			break;
		case MixerParameters::Am23:
			mixer.Am23 = value;
			break;
		case MixerParameters::Color:
			mixer.Color = (int)(value * 2.999);
			break;
		case MixerParameters::Fm12:
			mixer.Fm12 = value;
			break;
		case MixerParameters::Fm13:
			mixer.Fm13 = value;
			break;
		case MixerParameters::Fm23:
			mixer.Fm23 = value;
			break;
		case MixerParameters::Noise:
			mixer.Noise = value;
			break;
		}
	}

	void Voice::SetFilterHpParameter(Module module, FilterHpParameters parameter, double value)
	{
		if (parameter == FilterHpParameters::Env)
			modMatrix.FixedRoutes[ModMatrix::FixedRouteFilterHpEnv].Amount = value;
		else if (parameter == FilterHpParameters::Keytrack)
			modMatrix.FixedRoutes[ModMatrix::FixedRouteFilterHpKeytrack].Amount = -1.0f + value * 2.0;
		else
		{
			hpFilterL.SetParameter(parameter, value);
			hpFilterR.SetParameter(parameter, value);
		}
	}

	void Voice::SetFilterMainParameter(Module module, FilterMainParameters parameter, double value)
	{
		if (parameter == FilterMainParameters::Env)
			modMatrix.FixedRoutes[ModMatrix::FixedRouteFilterMainEnv].Amount = 10 * value;
		else if (parameter == FilterMainParameters::Keytrack)
			modMatrix.FixedRoutes[ModMatrix::FixedRouteFilterMainKeytrack].Amount = -1.0f + value * 2.0;
		else
		{
			mainFilterL.SetParameter(parameter, value);
			mainFilterR.SetParameter(parameter, value);
		}
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
