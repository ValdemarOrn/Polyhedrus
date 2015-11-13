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
				osc->Cent = (float)value;
				break;
			case OscParameters::Phase:
				osc->Phase = (float)value;
				break;
			case OscParameters::Shape:
				osc->Shape = (float)value;
				break;
			case OscParameters::Waveform:
				osc->SetWavetable(wavetableManager->LoadWavetable(Parameters::FloorToInt(value)));
				break;
			}
		};

		if (module == Module::Osc1)
		{
			if (parameter == OscParameters::Pan)
				mixer.Osc1Pan = (float)value;
			else if (parameter == OscParameters::Volume)
				mixer.Osc1Volume = (float)value;
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
				mixer.Osc2Pan = (float)value;
			else if (parameter == OscParameters::Volume)
				mixer.Osc2Volume = (float)value;
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
				mixer.Osc3Pan = (float)value;
			else if (parameter == OscParameters::Volume)
				mixer.Osc3Volume = (float)value;
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
			mixer.Am12 = (float)value;
			break;
		case MixerParameters::Am23:
			mixer.Am23 = (float)value;
			break;
		case MixerParameters::Color:
			noise.Type = (NoiseType)Parameters::RoundToInt(value);
			break;
		case MixerParameters::Fm12:
			mixer.Fm12 = (float)value;
			break;
		case MixerParameters::Fm13:
			mixer.Fm13 = (float)value;
			break;
		case MixerParameters::Fm23:
			mixer.Fm23 = (float)value;
			break;
		case MixerParameters::Noise:
			mixer.Noise = (float)value;
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
			modMatrix.FixedRoutes[ModMatrix::FixedRouteFilterHpEnv].Amount = (float)(10 * value);
		else if (parameter == FilterHpParameters::Keytrack)
			modMatrix.FixedRoutes[ModMatrix::FixedRouteFilterHpKeytrack].Amount = (float)value;
		else
		{
			hpFilterL.SetParameter(parameter, value);
			hpFilterR.SetParameter(parameter, value);
		}
	}

	void Voice::SetFilterMainParameter(Module module, FilterMainParameters parameter, double value)
	{
		if (parameter == FilterMainParameters::Env)
			modMatrix.FixedRoutes[ModMatrix::FixedRouteFilterMainEnv].Amount = (float)(10 * value);
		else if (parameter == FilterMainParameters::Keytrack)
			modMatrix.FixedRoutes[ModMatrix::FixedRouteFilterMainKeytrack].Amount = (float)value;
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

	void Voice::SetModParameter(Module module, ModParameters parameter, double value)
	{
		if (module == Module::Mod1)
			mod1.SetParameter(parameter, value);
		else if (module == Module::Mod2)
			mod2.SetParameter(parameter, value);
		else if(module == Module::Mod3)
			mod3.SetParameter(parameter, value);
	}

	void Voice::SetVoiceParameter(Module module, VoiceParameters parameter, double value)
	{
		switch (parameter)
		{
		case VoiceParameters::Bend:
			modMatrix.FixedRoutes[ModMatrix::FixedRouteOscAllPitchbend].Amount = (float)(Parameters::FloorToInt(value) / 24.0);
			break;
		case VoiceParameters::Detune:
			modMatrix.FixedRoutes[ModMatrix::FixedRouteOscAllUnisonDetune].Amount = (float)(value / 24.0);
			break;
		case VoiceParameters::Spread:
			modMatrix.FixedRoutes[ModMatrix::FixedRouteOscAllUnisonSpread].Amount = (float)value;
			break;
		case VoiceParameters::Glide:
			osc1.SetGlide((float)value);
			osc2.SetGlide((float)value);
			osc3.SetGlide((float)value);
			characterL.SetGlide((float)value);
			characterR.SetGlide((float)value);
			break;
		}
	}

	void Voice::SetModMatrixParameter(Module module, ModMatrixParameters parameter, double value)
	{
		int route = ((int)parameter) / 10 - 1;
		int subIdx = ((int)parameter) % 10;

		switch (subIdx)
		{
		case 0:
			this->modMatrix.Matrix[route].Source = (ModSource)Parameters::FloorToInt(value);
			break;
		case 1:
			this->modMatrix.Matrix[route].Destination = (ModDest)Parameters::FloorToInt(value);
			break;
		case 2:
			this->modMatrix.Matrix[route].ViaSource = (ModSource)Parameters::FloorToInt(value);
			break;
		case 3:
			this->modMatrix.Matrix[route].Amount = value;
			break;
		case 4:
			this->modMatrix.Matrix[route].ViaAmount = value;
			break;
		}

	}
}
