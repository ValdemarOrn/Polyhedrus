
#include "Synth.h"
#include "AudioLib/Utils.h"
#include "AudioLib/ValueTables.h"
#include "Fft/FastFFT.h"

extern "C"
{
	_declspec(dllexport) Synth* Create()
	{
		AudioLib::ValueTables::Init();
		AudioLib::Utils::Initialize();
		FastFFT<float>::Setup();
		Wavetable::Setup();

		return new Synth();
	}

	_declspec(dllexport) void Initialize(Synth* instance, int samplerate)
	{
		instance->Initialize(samplerate);
	}

	_declspec(dllexport) void SetParameter(Synth* instance, int parameter, double value)
	{
		instance->SetParameter(parameter, value);
	}

	_declspec(dllexport) void ProcessMidi(Synth* instance, uint8_t* message)
	{
		instance->ProcessMidi(message);
	}

	_declspec(dllexport) void ProcessAudio(Synth* instance, double** buffer, int bufferSize)
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

	_declspec(dllexport) void Delete(Synth* instance)
	{
		delete instance;
	}

	_declspec(dllexport) Wavetable* GetWavetable(Synth* instance, int tableIndex)
	{
		return Wavetable::Wavetables[tableIndex];
	}
}
