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
		this->Samplerate = samplerate;
		osc1.Initialize(samplerate, modulationUpdateRate);
		hpFilter.Initialize(samplerate, bufferSize, modulationUpdateRate);
		mainFilter.Initialize(samplerate, bufferSize, modulationUpdateRate);
		vca.Initialize(samplerate, bufferSize, modulationUpdateRate);
		ampEnv.Initialize(samplerate);
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

	void Voice::SetGate(float gate)
	{
		Velocity = gate;
	}

	void Voice::SetNote(int note)
	{
		Note = note;
		osc1.Note = note;
	}

	void Voice::SetPitchWheel(float pitchbend)
	{
		osc1.PitchBend = pitchbend;
	}

	void Voice::Process(float * buffer, int bufferSize)
	{
		osc1.GetSamples(buffer, bufferSize);
		hpFilter.Process(buffer, bufferSize);
		mainFilter.Process(hpFilter.GetOutput(), bufferSize);

		ampEnv.Gate = Velocity > 0;
		ampEnv.Process(bufferSize);
		vca.ControlVoltage = ampEnv.Output;
		vca.Process(mainFilter.GetOutput(), bufferSize);

		AudioLib::Utils::Copy(vca.GetOutput(), buffer, bufferSize);
		
	}
}
