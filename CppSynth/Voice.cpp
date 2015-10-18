#include "Voice.h"
#include "AudioLib/Utils.h"

using namespace AudioLib::Utils;

namespace Leiftur
{
	Voice::Voice()
	{
		oscMixL = 0;
		oscMixR = 0;
		outputL = 0;
		outputR = 0;
	}

	Voice::~Voice()
	{
		delete oscMixL;
		delete oscMixR;
		delete outputL;
		delete outputR;
	}

	void Voice::Initialize(int samplerate, int modulationUpdateRate, int bufferSize)
	{
		oscMixL = new float[bufferSize];
		oscMixR = new float[bufferSize];
		outputL = new float[bufferSize];
		outputR = new float[bufferSize];
		output[0] = outputL;
		output[1] = outputR;

		this->samplerate = samplerate;
		this->modulationUpdateRate = modulationUpdateRate;

		osc1.Initialize(samplerate, modulationUpdateRate, bufferSize);
		osc2.Initialize(samplerate, modulationUpdateRate, bufferSize);
		osc3.Initialize(samplerate, modulationUpdateRate, bufferSize);

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
		modEnv.Initialize(samplerate);
	}

	void Voice::SetParameter(Module module, int parameter, double value)
	{
		if (module == Module::Osc1 || module == Module::Osc2 || module == Module::Osc3)
			SetOscParameter(module, (OscParameters)parameter, value);
		if (module == Module::Mixer)
			SetMixerParameter(module, (MixerParameters)parameter, value);
		if (module == Module::FilterHp)
			SetFilterHpParameter(module, (FilterHpParameters)parameter, value);
		if (module == Module::FilterMain)
			SetFilterMainParameter(module, (FilterMainParameters)parameter, value);
		if (module == Module::EnvAmp || module == Module::EnvFilter || module == Module::EnvMod)
			SetEnvParameter(module, (EnvParameters)parameter, value);
	}

	void Voice::SetGate(float velocity)
	{
		Gate = velocity > 0;
		ampEnv.Gate = Gate;
		filterEnv.Gate = Gate;
		modEnv.Gate = Gate;
		modMatrix.ModSourceValues[(int)ModSource::Gate] = Gate;

		if (Gate)
		{
			this->velocity = velocity;
			ampEnv.Velocity = velocity;
			filterEnv.Velocity = velocity;
			modEnv.Velocity = velocity;
			modMatrix.ModSourceValues[(int)ModSource::Velocity] = velocity;
		}
	}

	void Voice::TurnOff()
	{
		ampEnv.Silence();
		filterEnv.Silence();
		modEnv.Silence();
	}

	void Voice::SetNote(int note)
	{
		this->Note = note;
		osc1.Note = note;
		osc2.Note = note;
		osc3.Note = note;
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

	void Voice::Process(int totalBufferSize)
	{
		int i = 0;
		int bufferSize = modulationUpdateRate;

		while (i < totalBufferSize)
		{
			ProcessModulation();

			osc1.Process(bufferSize);
			osc2.Process(bufferSize);
			osc3.Process(bufferSize);

			MixOscillators(bufferSize);

			hpFilterL.Process(oscMixL, bufferSize);
			hpFilterR.Process(oscMixR, bufferSize);
			mainFilterL.Process(hpFilterL.GetOutput(), bufferSize);
			mainFilterR.Process(hpFilterR.GetOutput(), bufferSize);

			ampEnv.Process(bufferSize);
			vcaOutputL.ControlVoltage = ampEnv.GetOutput();
			vcaOutputR.ControlVoltage = ampEnv.GetOutput();
			vcaOutputL.Process(mainFilterL.GetOutput(), bufferSize);
			vcaOutputR.Process(mainFilterR.GetOutput(), bufferSize);

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
		modMatrix.ModSourceValues[(int)ModSource::EnvMod] = modEnv.Process(modulationUpdateRate);

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

		//hpFilter.CutoffMod = modMatrix.ModDestinationValues[(int)ModDest::FilterHpCutoff];

		mainFilterL.SetCutoffMod(modMatrix.ModDestinationValues[(int)ModDest::FilterMainCutoff]);
		mainFilterR.SetCutoffMod(modMatrix.ModDestinationValues[(int)ModDest::FilterMainCutoff]);
		mainFilterL.SetDriveMod(modMatrix.ModDestinationValues[(int)ModDest::FilterMainDrive]);
		mainFilterR.SetDriveMod(modMatrix.ModDestinationValues[(int)ModDest::FilterMainDrive]);
		mainFilterL.SetResonanceMod(modMatrix.ModDestinationValues[(int)ModDest::FilterMainResonance]);
		mainFilterR.SetResonanceMod(modMatrix.ModDestinationValues[(int)ModDest::FilterMainResonance]);
		
	}
	void Voice::MixOscillators(int bufferSize)
	{
		float osc1Vol = LimitMin(mixer.Osc1Volume + mixer.Osc1VolumeMod, 0.0);
		float osc2Vol = LimitMin(mixer.Osc2Volume + mixer.Osc2VolumeMod, 0.0);
		float osc3Vol = LimitMin(mixer.Osc3Volume + mixer.Osc3VolumeMod, 0.0);
		float osc1Pan = mixer.Osc1Pan + mixer.Osc1PanMod;
		float osc2Pan = mixer.Osc2Pan + mixer.Osc2PanMod;
		float osc3Pan = mixer.Osc3Pan + mixer.Osc3PanMod;

		float osc1VolL = osc1Vol * Limit(-osc1Pan + 1, 0.0, 1.0);
		float osc1VolR = osc1Vol * Limit(osc1Pan + 1, 0.0, 1.0);

		float osc2VolL = osc2Vol * Limit(-osc2Pan + 1, 0.0, 1.0);
		float osc2VolR = osc2Vol * Limit(osc2Pan + 1, 0.0, 1.0);

		float osc3VolL = osc3Vol * Limit(-osc3Pan + 1, 0.0, 1.0);
		float osc3VolR = osc3Vol * Limit(osc3Pan + 1, 0.0, 1.0);

		ZeroBuffer(oscMixL, bufferSize);
		ZeroBuffer(oscMixR, bufferSize);
		GainAndSum(oscMixL, osc1.GetOutput(), osc1VolL, bufferSize);
		GainAndSum(oscMixR, osc1.GetOutput(), osc1VolR, bufferSize);
		GainAndSum(oscMixL, osc2.GetOutput(), osc2VolL, bufferSize);
		GainAndSum(oscMixR, osc2.GetOutput(), osc2VolR, bufferSize);
		GainAndSum(oscMixL, osc3.GetOutput(), osc3VolL, bufferSize);
		GainAndSum(oscMixR, osc3.GetOutput(), osc3VolR, bufferSize);
	}
}
