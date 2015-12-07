
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

void RunPerft1(int voiceCount, int seconds)
{
	const int samplerate = 48000;
	const int bufferSize = 64;
	float left[bufferSize];
	float right[bufferSize];
	float* buffer[2] = { left, right };
	uint8_t noteOnMessage[3] = { 0x90, 0x3C, 0x7F };

	auto synth = new Polyhedrus::Synth();
	synth->Initialize(samplerate, true, 0, 0);
	synth->SetParameter(Synth::PackParameter(Module::Voices, (int)VoiceParameters::Polyphony), voiceCount);
	synth->SetParameter(Synth::PackParameter(Module::Voices, (int)VoiceParameters::Unison), voiceCount);
	synth->ProcessMidi(noteOnMessage);

	int buffers = (10 * samplerate) / bufferSize; // 10 seconds of audio
	auto freq = Polyhedrus::PlatformSpecific::PerformanceFrequency();
	auto start = Polyhedrus::PlatformSpecific::PerformanceCounter();
	
	for (int i = 0; i < buffers; i++)
	{
		synth->ProcessAudio(buffer, bufferSize);
	}

	auto end = Polyhedrus::PlatformSpecific::PerformanceCounter();
	auto totalTimeUs = (end - start) / (freq / 1000);
	
	std::cout << "RunPerft1" << endl;
	cout << "Voices,Seconds,TimeMillis" << endl;
	std::cout << voiceCount << "," << seconds << "," << totalTimeUs << endl;
}


int main()
{
	auto dummy = Create();
	Delete(dummy);

	RunPerft1(1, 10);
	RunPerft1(10, 2);
	int exit;
	std::cin >> exit;
	return 0;
}