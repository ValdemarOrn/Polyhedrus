#include "Voice.h"

namespace Polyhedrus
{
	void Voice::SetOscParameter(Module module, OscParameters parameter, double value)
	{
		auto update = [=](shared_ptr<OscillatorBase> osc)
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
				osc->Cent = (int)value;
				break;
			case OscParameters::Phase:
				osc->Phase = (float)value;
				break;
			case OscParameters::Shape:
				osc->Shape = (float)value;
				break;
			case OscParameters::Keytrack:
				osc->Keytrack = (float)value;
				break;
			case OscParameters::GlideFactor:
				osc->GlideFactor = std::powf(10.0f, (float)(2.0 * value)) / 10.0f;
				osc->UpdateGlide();
				break;
			case OscParameters::BaseHz:
				osc->SetBaseHz(value);
				break;
			case OscParameters::Linear:
				osc->Linear = (float)(value * 1000);
				break;
			case OscParameters::Waveform:
				if (osc->GetType() == OscillatorType::Wavetable)
				{
					shared_ptr<OscillatorWt> wtOsc = dynamic_pointer_cast<OscillatorWt>(osc);
					wtOsc->SetWavetable(wavetableManager->LoadWavetable(Parameters::FloorToInt(value)));
				}
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
				modMatrix.FixedRoutes[ModMatrix::FixedRouteSlop1].Amount = (float)(value * 0.01666);
			else
				update(osc1);
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
				modMatrix.FixedRoutes[ModMatrix::FixedRouteSlop2].Amount = (float)(value * 0.00833);
			else
				update(osc2);
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
				modMatrix.FixedRoutes[ModMatrix::FixedRouteSlop3].Amount = (float)(value * 0.00833);
			else
				update(osc3);
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
			noise.Type = (float)value;
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
		case MixerParameters::NoiseRouting:
			mixer.NoiseRouting = (RoutingStage)Parameters::FloorToInt(value);
			break;
		case MixerParameters::CharacterOut:
			mixer.CharacterOut = (float)value;
			break;
		case MixerParameters::FilterHpOut:
			mixer.FilterHpOut = (float)value;
			break;
		case MixerParameters::FilterMainOut:
			mixer.FilterMainOut = (float)value;
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
			mainFilterL->IsEnabled = isEn;
			mainFilterR->IsEnabled = isEn;
			break;
		case ModuleSwitchParameters::DriveOn:
			driveL.IsEnabled = isEn;
			driveR.IsEnabled = isEn;
			break;
		case ModuleSwitchParameters::Mod1On:
			mod1.IsEnabled = isEn;
			break;
		case ModuleSwitchParameters::Mod2On:
			mod2.IsEnabled = isEn;
			break;
		case ModuleSwitchParameters::Mod3On:
			mod3.IsEnabled = isEn;
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
			modMatrix.FixedRoutes[ModMatrix::FixedRouteFilterHpEnv].Amount = (float)value;
		else if (parameter == FilterHpParameters::Keytrack)
			modMatrix.FixedRoutes[ModMatrix::FixedRouteFilterHpKeytrack].Amount = (float)value / 9.5f;
		else
		{
			hpFilterL.SetParameter(parameter, value);
			hpFilterR.SetParameter(parameter, value);
		}
	}

	void Voice::SetFilterMainParameter(Module module, FilterMainParameters parameter, double value)
	{
		if (parameter == FilterMainParameters::Env)
			modMatrix.FixedRoutes[ModMatrix::FixedRouteFilterMainEnv].Amount = (float)value;
		else if (parameter == FilterMainParameters::Keytrack)
			modMatrix.FixedRoutes[ModMatrix::FixedRouteFilterMainKeytrack].Amount = (float)value / 10.3f;
		else if (parameter == FilterMainParameters::Type)
		{
			auto type = (FilterMainType)Parameters::FloorToInt(value);
			if (type == FilterMainType::TrueZero)
			{
				mainFilterL = mainFilterTrueZeroL;
				mainFilterR = mainFilterTrueZeroR;
			}
			else if (type == FilterMainType::DualSvf)
			{
				mainFilterL = mainFilterSvfL;
				mainFilterR = mainFilterSvfR;
			}
		}
		else
		{
			mainFilterTrueZeroL->SetParameter(parameter, value);
			mainFilterTrueZeroR->SetParameter(parameter, value);
			mainFilterSvfL->SetParameter(parameter, value);
			mainFilterSvfR->SetParameter(parameter, value);
		}
	}

	void Voice::SetDriveParameter(Module mdoule, DriveParameters parameter, double value)
	{
		driveL.SetParameter(parameter, value);
		driveR.SetParameter(parameter, value);
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
			osc1->Glide = (float)value;
			osc2->Glide = (float)value;
			osc3->Glide = (float)value;
			osc1->UpdateGlide();
			osc2->UpdateGlide();
			osc3->UpdateGlide();
			characterL.SetGlide((float)value);
			characterR.SetGlide((float)value);
			break;
		}
	}
	
	void Voice::SetMacroParameter(Module module, MacroParameters parameter, double value)
	{
		macroParameters[(int)parameter - 1] = (float)value;
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
			this->modMatrix.Matrix[route].Amount = (float)value;
			break;
		case 4:
			this->modMatrix.Matrix[route].ViaAmount = (float)value;
			break;
		}

	}
}
