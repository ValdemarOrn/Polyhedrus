
#include <iostream>
#include "Synth.h"
#include "Parameters.h"
#include "AudioLib/Utils.h"
#include "AudioLib/Noise.h"
#include "AudioLib/ValueTables.h"
#include "AudioLib/FastSin.h"
#include "Fft/FastFFT.h"
#include "PlatformSpecific.h"

using namespace std;
using namespace Polyhedrus;

extern "C"
{
	_declspec(dllexport) Polyhedrus::Synth* Create()
	{
		Polyhedrus::Parameters::Init();
		AudioLib::ValueTables::Init();
		AudioLib::Utils::Initialize();
		AudioLib::Noise::Initialize();
		AudioLib::FastSin::Init();
		FastFFT<float>::Setup();
		
		return new Polyhedrus::Synth();
	}

	_declspec(dllexport) void Initialize(Polyhedrus::Synth* instance, int samplerate, int udpListenPort, int udpSendPort)
	{
		instance->Initialize(samplerate, true, udpListenPort, udpSendPort);
	}

	_declspec(dllexport) void SetParameter(Polyhedrus::Synth* instance, int parameter, double value)
	{
		instance->SetParameter(parameter, value);
	}

	_declspec(dllexport) void ProcessMidi(Polyhedrus::Synth* instance, uint8_t* message)
	{
		instance->ProcessMidi(message);
	}

	_declspec(dllexport) void ProcessAudio(Polyhedrus::Synth* instance, double** buffer, int bufferSize)
	{
		float l[1024];
		float r[1024];
		for (int i = 0; i < bufferSize; i++)
		{
			l[i] = (float)buffer[0][i];
			r[i] = (float)buffer[1][i];
		}

		float* lr[2];
		lr[0] = l;
		lr[1] = r;
		instance->ProcessAudio(lr, bufferSize);

		for (int i = 0; i < bufferSize; i++)
		{
			buffer[0][i] = l[i];
			buffer[1][i] = r[i];
		}
	}

	_declspec(dllexport) void Delete(Polyhedrus::Synth* instance)
	{
		delete instance;
	}


	std::shared_ptr<Polyhedrus::Wavetable> wt;

	_declspec(dllexport) Polyhedrus::Wavetable* GetWavetable(Polyhedrus::Synth* instance, int tableIndex)
	{
		Polyhedrus::WavetableManager wtman;
		wtman.Setup(Polyhedrus::PlatformSpecific::GetDllDir());
		wt = wtman.LoadWavetable(tableIndex);
		return &(*wt);
	}
}
