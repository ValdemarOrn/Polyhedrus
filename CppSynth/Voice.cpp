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
		velocity = velocity;
		ampEnv.Gate = velocity > 0;
		filterEnv.Gate = velocity > 0;
		modEnv.Gate = velocity > 0;
		modMatrix.ModSourceValues[(int)ModSource::Velocity] = velocity;
		modMatrix.ModSourceValues[(int)ModSource::Gate] = velocity > 0;
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
		osc1.PitchBend = pitchbend;
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
			osc1.Process(bufferSize);
			hpFilter.Process(osc1.GetOutput(), bufferSize);
			mainFilter.Process(hpFilter.GetOutput(), bufferSize);

			ampEnv.Process(bufferSize);
			vcaOutput.ControlVoltage = ampEnv.Output;
			vcaOutput.Process(mainFilter.GetOutput(), bufferSize);

			AudioLib::Utils::Copy(vcaOutput.GetOutput(), &outputBuffer[i], bufferSize);
			i += modulationUpdateRate;
		}
	}
}
