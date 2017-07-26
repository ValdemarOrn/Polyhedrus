#include "Voice.h"
#include "AudioLib/Utils.h"
#include "AudioLib/Sse.h"

using namespace AudioLib;

namespace Polyhedrus
{
	Voice::Voice()
	{
		VoiceNumber = 0;
		signalMixL = nullptr;
		signalMixR = nullptr;
		outputL = nullptr;
		outputR = nullptr;
		osc1Buffer = nullptr;
		osc2Buffer = nullptr;
		osc3Buffer = nullptr;

		osc1Wt = make_shared<OscillatorWt>();
		osc2Wt = make_shared<OscillatorWt>();
		osc3Wt = make_shared<OscillatorWt>();
		osc1 = osc1Wt;
		osc2 = osc2Wt;
		osc3 = osc3Wt;

		mainFilterSvfL = make_shared<FilterDualSvf>();
		mainFilterSvfR = make_shared<FilterDualSvf>();
		mainFilterTrueZeroL = make_shared<FilterTrueZero>();
		mainFilterTrueZeroR = make_shared<FilterTrueZero>();
		mainFilterL = mainFilterTrueZeroL;
		mainFilterR = mainFilterTrueZeroR;
	}

	Voice::~Voice()
	{
		delete[] signalMixL;
		delete[] signalMixR;
		delete[] outputL;
		delete[] outputR;
		delete[] osc1Buffer;
		delete[] osc2Buffer;
		delete[] osc3Buffer;
	}

	void Voice::Initialize(int samplerate, int modulationUpdateRate, int bufferSize, int voiceNumber, shared_ptr<WavetableManager> wavetableManager)
	{
		this->wavetableManager = wavetableManager;
		VoiceNumber = voiceNumber;

		modMatrix.voiceTuning = &voiceTuning[0];

		signalMixL = new float[bufferSize]();
		signalMixR = new float[bufferSize]();
		outputL = new float[bufferSize]();
		outputR = new float[bufferSize]();
		osc1Buffer = new float[bufferSize]();
		osc2Buffer = new float[bufferSize]();
		osc3Buffer = new float[bufferSize]();
		output[0] = outputL;
		output[1] = outputR;

		this->samplerate = samplerate;
		this->modulationUpdateRate = modulationUpdateRate;

		slopGen1.Initialize(samplerate, modulationUpdateRate, voiceNumber * 100 + 1);
		slopGen2.Initialize(samplerate, modulationUpdateRate, voiceNumber * 100 + 2);
		slopGen3.Initialize(samplerate, modulationUpdateRate, voiceNumber * 100 + 3);
		slopGen1.Fc = 0.5;
		slopGen2.Fc = 0.5;
		slopGen3.Fc = 0.5;

		osc1Wt->Initialize(samplerate, bufferSize, modulationUpdateRate);
		osc2Wt->Initialize(samplerate, bufferSize, modulationUpdateRate);
		osc3Wt->Initialize(samplerate, bufferSize, modulationUpdateRate);

		noise.Initialize(samplerate, bufferSize, voiceNumber);

		characterL.Initialize(samplerate, bufferSize, modulationUpdateRate);
		characterR.Initialize(samplerate, bufferSize, modulationUpdateRate);
		hpFilterL.Initialize(samplerate, bufferSize, modulationUpdateRate);
		hpFilterR.Initialize(samplerate, bufferSize, modulationUpdateRate);

		mainFilterSvfL->Initialize(samplerate, bufferSize, modulationUpdateRate);
		mainFilterSvfR->Initialize(samplerate, bufferSize, modulationUpdateRate);
		mainFilterTrueZeroL->Initialize(samplerate, bufferSize, modulationUpdateRate);
		mainFilterTrueZeroR->Initialize(samplerate, bufferSize, modulationUpdateRate);
		
		driveL.Initialize(samplerate, bufferSize);
		driveR.Initialize(samplerate, bufferSize);

		vcaOsc1.Initialize(samplerate, bufferSize, modulationUpdateRate);
		vcaOsc2.Initialize(samplerate, bufferSize, modulationUpdateRate);
		vcaOsc3.Initialize(samplerate, bufferSize, modulationUpdateRate);
		vcaOutputL.Initialize(samplerate, bufferSize, modulationUpdateRate);
		vcaOutputR.Initialize(samplerate, bufferSize, modulationUpdateRate);

		ampEnv.Initialize(samplerate);
		filterEnv.Initialize(samplerate);

		mod1.Initialize(samplerate);
		mod2.Initialize(samplerate);
		mod3.Initialize(samplerate);
	}

	void Voice::SetParameter(Module module, int parameter, double value)
	{
		if (module == Module::Osc1 || module == Module::Osc2 || module == Module::Osc3)
			SetOscParameter(module, (OscParameters)parameter, value);
		else if (module == Module::Mixer)
			SetMixerParameter(module, (MixerParameters)parameter, value);
		else if (module == Module::Character)
			SetCharacterParameter(module, (CharacterParameters)parameter, value);
		else if (module == Module::ModuleSwitches)
			SetModuleSwitchParameter(module, (ModuleSwitchParameters)parameter, value);
		else if (module == Module::Character)
			SetCharacterParameter(module, (CharacterParameters)parameter, value);
		else if (module == Module::FilterHp)
			SetFilterHpParameter(module, (FilterHpParameters)parameter, value);
		else if (module == Module::FilterMain)
			SetFilterMainParameter(module, (FilterMainParameters)parameter, value);
		else if (module == Module::Drive)
			SetDriveParameter(module, (DriveParameters)parameter, value);
		else if (module == Module::EnvAmp || module == Module::EnvFilter)
			SetEnvParameter(module, (EnvParameters)parameter, value);
		else if (module == Module::Mod1 || module == Module::Mod2 || module == Module::Mod3)
			SetModParameter(module, (ModParameters)parameter, value);
		else if (module == Module::Voices)
			SetVoiceParameter(module, (VoiceParameters)parameter, value);
		else if (module == Module::Macros)
			SetMacroParameter(module, (MacroParameters)parameter, value);
		else if (module == Module::ModMatrix)
			SetModMatrixParameter(module, (ModMatrixParameters)parameter, value);
	}

	void Voice::SetGate(float velocity)
	{
		Gate = velocity > 0;
		ampEnv.SetGate(Gate, velocity);
		filterEnv.SetGate(Gate, velocity);
		mod1.SetGate(Gate, velocity);
		mod2.SetGate(Gate, velocity);
		mod3.SetGate(Gate, velocity);
		modMatrix.ModSourceValues[(int)ModSource::Gate] = Gate;

		if (Gate)
		{
			this->velocity = velocity;
			osc1->Reset();
			osc2->Reset();
			osc3->Reset();
			mod1.Reset();
			mod2.Reset();
			mod3.Reset();
			modMatrix.ModSourceValues[(int)ModSource::Velocity] = velocity;
		}
	}

	void Voice::TurnOff()
	{
		ampEnv.Silence();
		filterEnv.Silence();
	}

	void Voice::SetBpm(double bpm)
	{
		this->bpm = bpm;
	}

	void Voice::SetNote(int note)
	{
		this->Note = note;
		osc1->Note = note;
		osc2->Note = note;
		osc3->Note = note;
		characterL.Note = note;
		characterR.Note = note;
		modMatrix.ModSourceValues[(int)ModSource::KeyTrack] = (float)((note - 60) / 12.0);
		modMatrix.ModSourceValues[(int)ModSource::KeyTrackUnipolar] = (float)(note / 12.0);
	}

	void Voice::SetPitchWheel(float pitchbend)
	{
		modMatrix.ModSourceValues[(int)ModSource::Pitchbend] = pitchbend;
	}

	void Voice::SetModWheel(float value)
	{
		modMatrix.ModSourceValues[(int)ModSource::ModWheel] = value;
	}

	void Voice::SetKeyPressure(float value)
	{
		modMatrix.ModSourceValues[(int)ModSource::KeyPressure] = value;
	}

	void Voice::SetChannelPressure(float value)
	{
		modMatrix.ModSourceValues[(int)ModSource::ChannelPressure] = value;
	}

	void Voice::SetUnisonValue(float value)
	{
		modMatrix.ModSourceValues[(int)ModSource::UnisonIndex] = value;
	}

	void Voice::Process(int totalBufferSize)
	{
		Sse::PreventDernormals();

		int i = 0;
		int bufferSize = modulationUpdateRate;
		float fmScaler = (float)M_PI;

		while (i < totalBufferSize)
		{
			ProcessModulation();
			mixer.ComputeOscVols();

			if (IsEnabled(ModuleSwitchParameters::Osc1On))
				osc1->Process(bufferSize);
			if (IsEnabled(ModuleSwitchParameters::Osc2On))
			{
				Utils::ZeroBuffer(osc2->FmBuffer, bufferSize);
				Utils::GainAndSum(osc1->GetOutput(), osc2->FmBuffer, mixer.Fm12Total * fmScaler, bufferSize);
				osc2->Process(bufferSize);
			}
			if (IsEnabled(ModuleSwitchParameters::Osc3On))
			{
				Utils::ZeroBuffer(osc3->FmBuffer, bufferSize);
				Utils::GainAndSum(osc1->GetOutput(), osc3->FmBuffer, mixer.Fm13Total * fmScaler, bufferSize);
				Utils::GainAndSum(osc2->GetOutput(), osc3->FmBuffer, mixer.Fm23Total * fmScaler, bufferSize);
				osc3->Process(bufferSize);
			}

			noise.Process(bufferSize);
			ProcessAm(bufferSize);

			MixSignals(bufferSize, RoutingStage::Character);
			characterL.Process(signalMixL, bufferSize);
			characterR.Process(signalMixR, bufferSize);

			MixSignals(bufferSize, RoutingStage::HpFilter);
			hpFilterL.Process(signalMixL, bufferSize);
			hpFilterR.Process(signalMixR, bufferSize);

			MixSignals(bufferSize, RoutingStage::MainFilter);
			mainFilterL->Process(signalMixL, bufferSize);
			mainFilterR->Process(signalMixR, bufferSize);

			MixSignals(bufferSize, RoutingStage::Drive);
			driveL.Process(signalMixL, bufferSize);
			driveR.Process(signalMixR, bufferSize);

			MixSignals(bufferSize, RoutingStage::Direct);
			vcaOutputL.Process(signalMixL, bufferSize);
			vcaOutputR.Process(signalMixR, bufferSize);

			AudioLib::Utils::Copy(vcaOutputL.GetOutput(), &outputL[i], bufferSize);
			AudioLib::Utils::Copy(vcaOutputR.GetOutput(), &outputR[i], bufferSize);
			i += bufferSize;
		}
	}

	void Voice::ProcessModulation()
	{
		mod1.Process(modulationUpdateRate);
		mod2.Process(modulationUpdateRate);
		mod3.Process(modulationUpdateRate);

		ampEnv.Process(modulationUpdateRate);
		filterEnv.Process(modulationUpdateRate);

		slopGen1.Process();
		slopGen2.Process();
		slopGen3.Process();

		modMatrix.ModSourceValues[(int)ModSource::EnvAmp] = ampEnv.GetOutput();
		modMatrix.ModSourceValues[(int)ModSource::EnvFilter] = filterEnv.GetOutput();
		modMatrix.ModSourceValues[(int)ModSource::Mod1] = mod1.Output;
		modMatrix.ModSourceValues[(int)ModSource::Mod2] = mod2.Output;
		modMatrix.ModSourceValues[(int)ModSource::Mod3] = mod3.Output;
		modMatrix.ModSourceValues[(int)ModSource::Mod1Unipolar] = mod1.OutputUnipolar;
		modMatrix.ModSourceValues[(int)ModSource::Mod2Unipolar] = mod2.OutputUnipolar;
		modMatrix.ModSourceValues[(int)ModSource::Mod3Unipolar] = mod3.OutputUnipolar;
		modMatrix.ModSourceValues[(int)ModSource::Mod1Env] = mod1.OutputEnv;
		modMatrix.ModSourceValues[(int)ModSource::Mod2Env] = mod2.OutputEnv;
		modMatrix.ModSourceValues[(int)ModSource::Mod3Env] = mod3.OutputEnv;
		modMatrix.ModSourceValues[(int)ModSource::SlopGen1] = slopGen1.Output;
		modMatrix.ModSourceValues[(int)ModSource::SlopGen2] = slopGen2.Output;
		modMatrix.ModSourceValues[(int)ModSource::SlopGen3] = slopGen3.Output;
		modMatrix.ModSourceValues[(int)ModSource::Macro1] = macroParameters[0];
		modMatrix.ModSourceValues[(int)ModSource::Macro2] = macroParameters[1];
		modMatrix.ModSourceValues[(int)ModSource::Macro3] = macroParameters[2];
		modMatrix.ModSourceValues[(int)ModSource::Macro4] = macroParameters[3];
		modMatrix.ModSourceValues[(int)ModSource::Macro5] = macroParameters[4];
		modMatrix.ModSourceValues[(int)ModSource::Macro6] = macroParameters[5];
		modMatrix.ModSourceValues[(int)ModSource::Macro7] = macroParameters[6];
		modMatrix.ModSourceValues[(int)ModSource::Macro8] = macroParameters[7];
		modMatrix.Process();

		osc1->PitchMod = modMatrix.ModDestinationValues[(int)ModDest::Osc1Pitch] * 3.0f;
		osc1->ShapeMod = modMatrix.ModDestinationValues[(int)ModDest::Osc1Shape];
		mixer.Osc1PanMod = modMatrix.ModDestinationValues[(int)ModDest::Osc1Pan];
		mixer.Osc1VolumeMod = modMatrix.ModDestinationValues[(int)ModDest::Osc1Volume];

		osc2->PitchMod = modMatrix.ModDestinationValues[(int)ModDest::Osc2Pitch] * 3.0f;
		osc2->ShapeMod = modMatrix.ModDestinationValues[(int)ModDest::Osc2Shape];
		mixer.Osc2PanMod = modMatrix.ModDestinationValues[(int)ModDest::Osc2Pan];
		mixer.Osc2VolumeMod = modMatrix.ModDestinationValues[(int)ModDest::Osc2Volume];

		osc3->PitchMod = modMatrix.ModDestinationValues[(int)ModDest::Osc3Pitch] * 3.0f;
		osc3->ShapeMod = modMatrix.ModDestinationValues[(int)ModDest::Osc3Shape];
		mixer.Osc3PanMod = modMatrix.ModDestinationValues[(int)ModDest::Osc3Pan];
		mixer.Osc3VolumeMod = modMatrix.ModDestinationValues[(int)ModDest::Osc3Volume];

		mixer.Am12Mod = modMatrix.ModDestinationValues[(int)ModDest::MixerAm12];
		mixer.Am23Mod = modMatrix.ModDestinationValues[(int)ModDest::MixerAm23];
		mixer.Fm12Mod = modMatrix.ModDestinationValues[(int)ModDest::MixerFm12];
		mixer.Fm13Mod = modMatrix.ModDestinationValues[(int)ModDest::MixerFm13];
		mixer.Fm23Mod = modMatrix.ModDestinationValues[(int)ModDest::MixerFm23];
		mixer.NoiseMod = modMatrix.ModDestinationValues[(int)ModDest::MixerNoise];
		mixer.CharacterOutMod = modMatrix.ModDestinationValues[(int)ModDest::MixerCharacterOut];
		mixer.FilterHpOutMod = modMatrix.ModDestinationValues[(int)ModDest::MixerFilterHpOut];
		mixer.FilterMainOutMod = modMatrix.ModDestinationValues[(int)ModDest::MixerFilterMainOut];

		hpFilterL.CutoffMod = modMatrix.ModDestinationValues[(int)ModDest::FilterHpCutoff] * 9.5f;
		hpFilterR.CutoffMod = modMatrix.ModDestinationValues[(int)ModDest::FilterHpCutoff] * 9.5f;

		mainFilterL->SetCutoffMod(modMatrix.ModDestinationValues[(int)ModDest::FilterMainCutoff] * 10.3f) ;
		mainFilterR->SetCutoffMod(modMatrix.ModDestinationValues[(int)ModDest::FilterMainCutoff] * 10.3f);
		mainFilterL->SetDriveMod(modMatrix.ModDestinationValues[(int)ModDest::FilterMainDrive]);
		mainFilterR->SetDriveMod(modMatrix.ModDestinationValues[(int)ModDest::FilterMainDrive]);
		mainFilterL->SetResonanceMod(modMatrix.ModDestinationValues[(int)ModDest::FilterMainResonance]);
		mainFilterR->SetResonanceMod(modMatrix.ModDestinationValues[(int)ModDest::FilterMainResonance]);

		// set env modulation

		// Non-matrix Modulation parameters - The destinations are not available via the matrix; assign directly instead
		vcaOutputL.ControlVoltage = modMatrix.ModSourceValues[(int)ModSource::EnvAmp];
		vcaOutputR.ControlVoltage = modMatrix.ModSourceValues[(int)ModSource::EnvAmp];
	}

	void Voice::ProcessAm(int bufferSize)
	{
		float* osc1Out = osc1->GetOutput();
		float* osc2Out = osc2->GetOutput();
		float* osc3Out = osc3->GetOutput();

		for (int i = 0; i < bufferSize; i++)
		{
			osc1Buffer[i] = osc1Out[i];
			osc2Buffer[i] = osc2Out[i] * (1 + 2 * osc1Out[i] * mixer.Am12Total);
			osc3Buffer[i] = osc3Out[i] * (1 + 2 * osc2Out[i] * mixer.Am23Total);
		}
	}

	void Voice::MixSignals(int bufferSize, RoutingStage stage)
	{
		Utils::PreventDenormal(signalMixL, bufferSize);
		Utils::PreventDenormal(signalMixR, bufferSize);

		if (stage == RoutingStage::HpFilter)
		{
			Utils::GainAndSum(characterL.GetOutput(), signalMixL, mixer.CharacterOutTotal, bufferSize);
			Utils::GainAndSum(characterR.GetOutput(), signalMixR, mixer.CharacterOutTotal, bufferSize);
		}
		else if (stage == RoutingStage::MainFilter)
		{
			Utils::GainAndSum(hpFilterL.GetOutput(), signalMixL, mixer.FilterHpOutTotal, bufferSize);
			Utils::GainAndSum(hpFilterR.GetOutput(), signalMixR, mixer.FilterHpOutTotal, bufferSize);
		}
		else if (stage == RoutingStage::Drive)
		{
			Utils::GainAndSum(mainFilterL->GetOutput(), signalMixL, mixer.FilterMainOutTotal, bufferSize);
			Utils::GainAndSum(mainFilterR->GetOutput(), signalMixR, mixer.FilterMainOutTotal, bufferSize);
		}
		else if (stage == RoutingStage::Direct)
		{
			Utils::GainAndSum(driveL.GetOutput(), signalMixL, 1.0, bufferSize);
			Utils::GainAndSum(driveR.GetOutput(), signalMixR, 1.0, bufferSize);
		}

		if (mixer.NoiseRouting == stage)
		{
			Utils::GainAndSum(noise.GetOutput()[0], signalMixL, mixer.NoiseTotal, bufferSize);
			Utils::GainAndSum(noise.GetOutput()[1], signalMixR, mixer.NoiseTotal, bufferSize);
		}
		if (IsEnabled(ModuleSwitchParameters::Osc1On) && mixer.Osc1Routing == stage)
		{
			Utils::GainAndSum(osc1Buffer, signalMixL, mixer.Osc1VolL, bufferSize);
			Utils::GainAndSum(osc1Buffer, signalMixR, mixer.Osc1VolR, bufferSize);
		}
		if (IsEnabled(ModuleSwitchParameters::Osc2On) && mixer.Osc2Routing == stage)
		{
			Utils::GainAndSum(osc2Buffer, signalMixL, mixer.Osc2VolL, bufferSize);
			Utils::GainAndSum(osc2Buffer, signalMixR, mixer.Osc2VolR, bufferSize);
		}
		if (IsEnabled(ModuleSwitchParameters::Osc3On) && mixer.Osc3Routing == stage)
		{
			Utils::GainAndSum(osc3Buffer, signalMixL, mixer.Osc3VolL, bufferSize);
			Utils::GainAndSum(osc3Buffer, signalMixR, mixer.Osc3VolR, bufferSize);
		}
	}

	inline bool Voice::IsEnabled(ModuleSwitchParameters module)
	{
		return moduleSwitches[(int)module];
	}
}
