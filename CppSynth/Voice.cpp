#include "Voice.h"
#include "AudioLib/Utils.h"

namespace Leiftur
{
	Voice::Voice()
	{
	}

	Voice::~Voice()
	{
	}

	void Voice::Initialize(int samplerate, int modulationUpdateRate, int bufferSize)
	{
		this->samplerate = samplerate;
		this->modulationUpdateRate = modulationUpdateRate;

		osc1.Initialize(samplerate, modulationUpdateRate, bufferSize);
		osc2.Initialize(samplerate, modulationUpdateRate, bufferSize);
		osc3.Initialize(samplerate, modulationUpdateRate, bufferSize);

		hpFilter.Initialize(samplerate, bufferSize, modulationUpdateRate);
		mainFilter.Initialize(samplerate, bufferSize, modulationUpdateRate);

		vcaOsc1.Initialize(samplerate, bufferSize, modulationUpdateRate);
		vcaOsc2.Initialize(samplerate, bufferSize, modulationUpdateRate);
		vcaOsc3.Initialize(samplerate, bufferSize, modulationUpdateRate);
		vcaOutput.Initialize(samplerate, bufferSize, modulationUpdateRate);

		ampEnv.Initialize(samplerate);
		filterEnv.Initialize(samplerate);
		modEnv.Initialize(samplerate);
	}

	void Voice::SetParameter(Module module, int parameter, double value)
	{
		if (module == Module::Osc1 && parameter == (int)OscParameters::Shape)
			osc1.Shape = value;
		if (module == Module::FilterMain)
			mainFilter.SetParameter((FilterMainParameters)parameter, value);
		if (module == Module::FilterHp)
			hpFilter.SetParameter((FilterHpParameters)parameter, value);
		if (module == Module::EnvAmp)
			ampEnv.SetParameter((EnvParameters)parameter, value);
	}

	void Voice::SetGate(float velocity)
	{
		gate = velocity > 0;
		ampEnv.Gate = gate;
		filterEnv.Gate = gate;
		modEnv.Gate = gate;
		modMatrix.ModSourceValues[(int)ModSource::Gate] = gate;

		if (gate)
		{
			this->velocity = velocity;
			ampEnv.Velocity = velocity;
			filterEnv.Velocity = velocity;
			modEnv.Velocity = velocity;
			modMatrix.ModSourceValues[(int)ModSource::Velocity] = velocity;
		}
	}

	void Voice::SetNote(int note)
	{
		this->Note = note;
		osc1.Note = note;
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

	void Voice::Process(float * outputBuffer, int totalBufferSize)
	{
		int i = 0;
		int bufferSize = modulationUpdateRate;

		while (i < totalBufferSize)
		{
			ProcessModulation();

			osc1.Process(bufferSize);
			hpFilter.Process(osc1.GetOutput(), bufferSize);
			mainFilter.Process(hpFilter.GetOutput(), bufferSize);

			ampEnv.Process(bufferSize);
			vcaOutput.ControlVoltage = ampEnv.GetOutput();
			vcaOutput.Process(mainFilter.GetOutput(), bufferSize);

			AudioLib::Utils::Copy(vcaOutput.GetOutput(), &outputBuffer[i], bufferSize);
			i += modulationUpdateRate;
		}
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

		mixer.MixerAm12Mod = modMatrix.ModDestinationValues[(int)ModDest::MixerAm12];
		mixer.MixerAm23Mod = modMatrix.ModDestinationValues[(int)ModDest::MixerAm23];
		mixer.MixerFm12Mod = modMatrix.ModDestinationValues[(int)ModDest::MixerFm12];
		mixer.MixerFm13Mod = modMatrix.ModDestinationValues[(int)ModDest::MixerFm13];
		mixer.MixerFm23Mod = modMatrix.ModDestinationValues[(int)ModDest::MixerFm23];
		mixer.MixerNoiseMod = modMatrix.ModDestinationValues[(int)ModDest::MixerNoise];
		mixer.MixerOutputMod = modMatrix.ModDestinationValues[(int)ModDest::MixerOutput];

		//hpFilter.CutoffMod = modMatrix.ModDestinationValues[(int)ModDest::FilterHpCutoff];

		//mainFilter.SetCutoffMod(modMatrix.ModDestinationValues[(int)ModDest::FilterMainCutoff]);
		//mainFilter.SetDriveMod(modMatrix.ModDestinationValues[(int)ModDest::FilterMainDrive]);
		//mainFilter.SetResonanceMod(modMatrix.ModDestinationValues[(int)ModDest::FilterMainResonance]);
		
	}
}
