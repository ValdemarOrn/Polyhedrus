#include "Voice.h"
#include "AudioLib/Utils.h"

using namespace AudioLib::Utils;

namespace Leiftur
{
	Voice::Voice()
	{
		signalMixL = 0;
		signalMixR = 0;
		outputL = 0;
		outputR = 0;
	}

	Voice::~Voice()
	{
		delete signalMixL;
		delete signalMixR;
		delete outputL;
		delete outputR;
	}

	void Voice::Initialize(int samplerate, int modulationUpdateRate, int bufferSize, shared_ptr<WavetableManager> wavetableManager)
	{
		this->wavetableManager = wavetableManager;

		signalMixL = new float[bufferSize];
		signalMixR = new float[bufferSize];
		outputL = new float[bufferSize];
		outputR = new float[bufferSize];
		output[0] = outputL;
		output[1] = outputR;

		this->samplerate = samplerate;
		this->modulationUpdateRate = modulationUpdateRate;

		osc1.Initialize(samplerate, bufferSize, modulationUpdateRate);
		osc2.Initialize(samplerate, bufferSize, modulationUpdateRate);
		osc3.Initialize(samplerate, bufferSize, modulationUpdateRate);

		characterL.Initialize(samplerate, bufferSize, modulationUpdateRate);
		characterR.Initialize(samplerate, bufferSize, modulationUpdateRate);
		hpFilterL.Initialize(samplerate, bufferSize, modulationUpdateRate);
		hpFilterR.Initialize(samplerate, bufferSize, modulationUpdateRate);
		mainFilterL.Initialize(samplerate, bufferSize, modulationUpdateRate);
		mainFilterR.Initialize(samplerate, bufferSize, modulationUpdateRate);

		vcaOsc1.Initialize(samplerate, bufferSize, modulationUpdateRate);
		vcaOsc2.Initialize(samplerate, bufferSize, modulationUpdateRate);
		vcaOsc3.Initialize(samplerate, bufferSize, modulationUpdateRate);
		vcaOutputL.Initialize(samplerate, bufferSize, modulationUpdateRate);
		vcaOutputR.Initialize(samplerate, bufferSize, modulationUpdateRate);

		ampEnv.Initialize(samplerate);
		filterEnv.Initialize(samplerate);
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
		else if (module == Module::EnvAmp || module == Module::EnvFilter)
			SetEnvParameter(module, (EnvParameters)parameter, value);
		else if (module == Module::Voices)
			SetVoiceParameter(module, (VoiceParameters)parameter, value);
	}

	void Voice::SetGate(float velocity)
	{
		Gate = velocity > 0;
		ampEnv.SetGate(Gate);
		filterEnv.SetGate(Gate);
		modMatrix.ModSourceValues[(int)ModSource::Gate] = Gate;

		if (Gate)
		{
			this->velocity = velocity;
			ampEnv.Velocity = velocity;
			filterEnv.Velocity = velocity;
			osc1.Reset();
			osc2.Reset();
			osc3.Reset();
			modMatrix.ModSourceValues[(int)ModSource::Velocity] = velocity;
		}
	}

	void Voice::TurnOff()
	{
		ampEnv.Silence();
		filterEnv.Silence();
	}

	void Voice::SetNote(int note)
	{
		this->Note = note;
		osc1.Note = note;
		osc2.Note = note;
		osc3.Note = note;
		characterL.Note = note;
		characterR.Note = note;
		modMatrix.ModSourceValues[(int)ModSource::KeyTrack] = (note - 60) / 12.0;
		modMatrix.ModSourceValues[(int)ModSource::KeyTrackUnipolar] = note / 12.0;
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
		int i = 0;
		int bufferSize = modulationUpdateRate;

		if (Gate)
		{
			int k = 23;
		}

		while (i < totalBufferSize)
		{
			ProcessModulation();

			if (IsEnabled(ModuleSwitchParameters::Osc1On))
				osc1.Process(bufferSize);
			if (IsEnabled(ModuleSwitchParameters::Osc2On))
				osc2.Process(bufferSize);
			if (IsEnabled(ModuleSwitchParameters::Osc3On))
				osc3.Process(bufferSize);

			MixSignals(bufferSize, RoutingStage::Character);
			characterL.Process(signalMixL, bufferSize);
			characterR.Process(signalMixR, bufferSize);

			MixSignals(bufferSize, RoutingStage::HpFilter);
			hpFilterL.Process(signalMixL, bufferSize);
			hpFilterR.Process(signalMixR, bufferSize);

			MixSignals(bufferSize, RoutingStage::MainFilter);
			mainFilterL.Process(signalMixL, bufferSize);
			mainFilterR.Process(signalMixR, bufferSize);

			MixSignals(bufferSize, RoutingStage::Direct);
			vcaOutputL.Process(signalMixL, bufferSize);
			vcaOutputR.Process(signalMixR, bufferSize);

			AudioLib::Utils::Copy(vcaOutputL.GetOutput(), &outputL[i], bufferSize);
			AudioLib::Utils::Copy(vcaOutputR.GetOutput(), &outputR[i], bufferSize);
			i += modulationUpdateRate;
		}
	}

	float** Voice::GetOutput()
	{
		return output;
	}

	void Voice::ProcessModulation()
	{
		modMatrix.ModSourceValues[(int)ModSource::EnvAmp] = ampEnv.Process(modulationUpdateRate);
		modMatrix.ModSourceValues[(int)ModSource::EnvFilter] = filterEnv.Process(modulationUpdateRate);

		modMatrix.Process();

		osc1.PitchMod = modMatrix.ModDestinationValues[(int)ModDest::Osc1Pitch];
		osc1.ShapeMod = modMatrix.ModDestinationValues[(int)ModDest::Osc1Shape];
		mixer.Osc1PanMod = modMatrix.ModDestinationValues[(int)ModDest::Osc1Pan];
		mixer.Osc1VolumeMod = modMatrix.ModDestinationValues[(int)ModDest::Osc1Volume];

		osc2.PitchMod = modMatrix.ModDestinationValues[(int)ModDest::Osc2Pitch];
		osc2.ShapeMod = modMatrix.ModDestinationValues[(int)ModDest::Osc2Shape];
		mixer.Osc2PanMod = modMatrix.ModDestinationValues[(int)ModDest::Osc2Pan];
		mixer.Osc2VolumeMod = modMatrix.ModDestinationValues[(int)ModDest::Osc2Volume];

		osc3.PitchMod = modMatrix.ModDestinationValues[(int)ModDest::Osc3Pitch];
		osc3.ShapeMod = modMatrix.ModDestinationValues[(int)ModDest::Osc3Shape];
		mixer.Osc3PanMod = modMatrix.ModDestinationValues[(int)ModDest::Osc3Pan];
		mixer.Osc3VolumeMod = modMatrix.ModDestinationValues[(int)ModDest::Osc3Volume];

		mixer.Am12Mod = modMatrix.ModDestinationValues[(int)ModDest::MixerAm12];
		mixer.Am23Mod = modMatrix.ModDestinationValues[(int)ModDest::MixerAm23];
		mixer.Fm12Mod = modMatrix.ModDestinationValues[(int)ModDest::MixerFm12];
		mixer.Fm13Mod = modMatrix.ModDestinationValues[(int)ModDest::MixerFm13];
		mixer.Fm23Mod = modMatrix.ModDestinationValues[(int)ModDest::MixerFm23];
		mixer.NoiseMod = modMatrix.ModDestinationValues[(int)ModDest::MixerNoise];
		mixer.OutputMod = modMatrix.ModDestinationValues[(int)ModDest::MixerOutput];

		hpFilterL.CutoffMod = modMatrix.ModDestinationValues[(int)ModDest::FilterHpCutoff];
		hpFilterR.CutoffMod = modMatrix.ModDestinationValues[(int)ModDest::FilterHpCutoff];

		mainFilterL.SetCutoffMod(modMatrix.ModDestinationValues[(int)ModDest::FilterMainCutoff]);
		mainFilterR.SetCutoffMod(modMatrix.ModDestinationValues[(int)ModDest::FilterMainCutoff]);
		mainFilterL.SetDriveMod(modMatrix.ModDestinationValues[(int)ModDest::FilterMainDrive]);
		mainFilterR.SetDriveMod(modMatrix.ModDestinationValues[(int)ModDest::FilterMainDrive]);
		mainFilterL.SetResonanceMod(modMatrix.ModDestinationValues[(int)ModDest::FilterMainResonance]);
		mainFilterR.SetResonanceMod(modMatrix.ModDestinationValues[(int)ModDest::FilterMainResonance]);

		// Non-matrix Modulation parameters - The destinations are not available via the matrix; assign directly instead
		vcaOutputL.ControlVoltage = modMatrix.ModSourceValues[(int)ModSource::EnvAmp];
		vcaOutputR.ControlVoltage = modMatrix.ModSourceValues[(int)ModSource::EnvAmp];
	}

	void Voice::MixSignals(int bufferSize, RoutingStage stage)
	{
		mixer.ComputeOscVols();

		PreventDenormal(signalMixL, bufferSize);
		PreventDenormal(signalMixR, bufferSize);

		if (stage == RoutingStage::HpFilter)
		{
			Copy(characterL.GetOutput(), signalMixL, bufferSize);
			Copy(characterR.GetOutput(), signalMixR, bufferSize);
		}
		else if (stage == RoutingStage::MainFilter)
		{
			Copy(hpFilterL.GetOutput(), signalMixL, bufferSize);
			Copy(hpFilterR.GetOutput(), signalMixR, bufferSize);
		}
		else if (stage == RoutingStage::Direct)
		{
			Copy(mainFilterL.GetOutput(), signalMixL, bufferSize);
			Copy(mainFilterR.GetOutput(), signalMixR, bufferSize);
		}


		if (IsEnabled(ModuleSwitchParameters::Osc1On) && mixer.Osc1Routing == stage)
		{
			GainAndSum(osc1.GetOutput(), signalMixL, mixer.Osc1VolL, bufferSize);
			GainAndSum(osc1.GetOutput(), signalMixR, mixer.Osc1VolR, bufferSize);
		}

		if (IsEnabled(ModuleSwitchParameters::Osc2On) && mixer.Osc2Routing == stage)
		{
			GainAndSum(osc2.GetOutput(), signalMixL, mixer.Osc2VolL, bufferSize);
			GainAndSum(osc2.GetOutput(), signalMixR, mixer.Osc2VolR, bufferSize);
		}

		if (IsEnabled(ModuleSwitchParameters::Osc3On) && mixer.Osc3Routing == stage)
		{
			GainAndSum(osc3.GetOutput(), signalMixL, mixer.Osc3VolL, bufferSize);
			GainAndSum(osc3.GetOutput(), signalMixR, mixer.Osc3VolR, bufferSize);
		}
	}

	inline bool Voice::IsEnabled(ModuleSwitchParameters module)
	{
		return moduleSwitches[(int)module];
	}
}
