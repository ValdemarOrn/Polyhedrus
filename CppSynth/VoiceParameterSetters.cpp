#include "Voice.h"

namespace Leiftur
{
	void Voice::SetOscParameter(Module module, OscParameters parameter, double value)
	{
		auto update = [=](Oscillator* osc)
		{
			switch (parameter)
			{
			case OscParameters::Octave:
				osc->Octave = Parameters::FloorToInt(value);
				break;
			case OscParameters::Semi:
				osc->Semi = Parameters::FloorToInt(value);
				break;
			case OscParameters::Cent:
				osc->Cent = value;
				break;
			case OscParameters::Phase:
				osc->Phase = value;
				break;
			case OscParameters::Shape:
				osc->Shape = value;
				break;
			case OscParameters::Waveform:
				osc->SetWavetable(wavetableManager->LoadWavetable(Parameters::FloorToInt(value)));
				break;
			}
		};

		if (module == Module::Osc1)
		{
			if (parameter == OscParameters::Pan)
				mixer.Osc1Pan = value;
			else if (parameter == OscParameters::Volume)
				mixer.Osc1Volume = value;
			else if (parameter == OscParameters::Routing)
				mixer.Osc1Routing = (RoutingStage)Parameters::FloorToInt(value);
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
				mixer.Osc2Pan = value;
			else if (parameter == OscParameters::Volume)
				mixer.Osc2Volume = value;
			else if (parameter == OscParameters::Routing)
				mixer.Osc2Routing = (RoutingStage)Parameters::FloorToInt(value);
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
				mixer.Osc3Pan = value;
			else if (parameter == OscParameters::Volume)
				mixer.Osc3Volume = value;
			else if (parameter == OscParameters::Routing)
				mixer.Osc3Routing = (RoutingStage)Parameters::FloorToInt(value);
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
			mixer.Color = Parameters::RoundToInt(value);
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

	void Voice::SetModuleSwitchParameter(Module module, ModuleSwitchParameters parameter, double value)
	{
		bool isEn = value >= 0.5;

		moduleSwitches[(int)parameter] = isEn;
		switch (parameter)
		{
		case ModuleSwitchParameters::CharacterOn:
			characterL.IsEnabled = isEn;
			characterR.IsEnabled = isEn;
			break;
		case ModuleSwitchParameters::FilterHpOn:
			hpFilterL.IsEnabled = isEn;
			hpFilterR.IsEnabled = isEn;
			break;
		case ModuleSwitchParameters::FilterMainOn:
			mainFilterL.IsEnabled = isEn;
			mainFilterR.IsEnabled = isEn;
			break;
		}
	}

	void Voice::SetCharacterParameter(Module module, CharacterParameters parameter, double value)
	{
		characterL.SetParameter(parameter, value);
		characterR.SetParameter(parameter, value);
	}

	void Voice::SetFilterHpParameter(Module module, FilterHpParameters parameter, double value)
	{
		if (parameter == FilterHpParameters::Env)
			modMatrix.FixedRoutes[ModMatrix::FixedRouteFilterHpEnv].Amount = 10 * value;
		else if (parameter == FilterHpParameters::Keytrack)
			modMatrix.FixedRoutes[ModMatrix::FixedRouteFilterHpKeytrack].Amount = value;
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
			modMatrix.FixedRoutes[ModMatrix::FixedRouteFilterMainKeytrack].Amount = value;
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
	}

	void Voice::SetVoiceParameter(Module module, VoiceParameters parameter, double value)
	{
		switch (parameter)
		{
		case VoiceParameters::Bend:
			modMatrix.FixedRoutes[ModMatrix::FixedRouteOscAllPitchbend].Amount = Parameters::FloorToInt(value) / 24.0;
			break;
		case VoiceParameters::Detune:
			modMatrix.FixedRoutes[ModMatrix::FixedRouteOscAllUnisonDetune].Amount = value / 12.0;
			break;
		case VoiceParameters::Spread:
			modMatrix.FixedRoutes[ModMatrix::FixedRouteOscAllUnisonSpread].Amount = value;
			break;
		case VoiceParameters::Glide:
			osc1.SetGlide(value);
			osc2.SetGlide(value);
			osc3.SetGlide(value);
			characterL.SetGlide(value);
			characterR.SetGlide(value);
			break;
		}
	}
}
