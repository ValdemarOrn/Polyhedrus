
#include "Synth.h"
#include "AudioLib/Utils.h"
#include "AudioLib/ValueTables.h"
#include "Fft/FastFFT.h"

extern "C"
{
	_declspec(dllexport) Leiftur::Synth* Create()
	{
		AudioLib::ValueTables::Init();
		AudioLib::Utils::Initialize();
		FastFFT<float>::Setup();
		Leiftur::Wavetable::Setup();

		return new Leiftur::Synth();
	}

	_declspec(dllexport) void Initialize(Leiftur::Synth* instance, int samplerate, int udpListenPort, int udpSendPort)
	{
		instance->Initialize(samplerate, udpListenPort, udpSendPort);
	}

	_declspec(dllexport) void SetParameter(Leiftur::Synth* instance, int parameter, double value)
	{
		instance->SetParameter(parameter, value);
	}

	_declspec(dllexport) void ProcessMidi(Leiftur::Synth* instance, uint8_t* message)
	{
		instance->ProcessMidi(message);
	}

	_declspec(dllexport) void ProcessAudio(Leiftur::Synth* instance, double** buffer, int bufferSize)
	{
		float l[1024];
		float r[1024];
		for (size_t i = 0; i < bufferSize; i++)
		{
			l[i] = buffer[0][i];
			r[i] = buffer[1][i];
		}

		float* lr[2];
		lr[0] = l;
		lr[1] = r;
		instance->ProcessAudio(lr, bufferSize);

		for (size_t i = 0; i < bufferSize; i++)
		{
			buffer[0][i] = l[i];
			buffer[1][i] = l[i];
		}
	}

	_declspec(dllexport) void Delete(Leiftur::Synth* instance)
	{
		delete instance;
	}

	_declspec(dllexport) Leiftur::Wavetable* GetWavetable(Leiftur::Synth* instance, int tableIndex)
	{
		return Leiftur::Wavetable::Wavetables[tableIndex];
	}
}
