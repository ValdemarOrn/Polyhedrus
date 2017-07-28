#include <iostream>
#include "Synth.h"
#include "Parameters.h"
#include "AudioLib/Utils.h"
#include "AudioLib/Noise.h"
#include "AudioLib/ValueTables.h"
#include "AudioLib/FastSin.h"
#include "Fft/FastFFT.h"
#include "PlatformSpecific.h"
#include "AudioLib/LcgRandom.h"
#include "AudioLib/Sse.h"
#include "AudioLib/Decimator.h"

using namespace std;
using namespace Polyhedrus;

void Init()
{
	Polyhedrus::Parameters::Init();
	AudioLib::ValueTables::Init();
	AudioLib::Utils::Initialize();
	AudioLib::Noise::Initialize();
	AudioLib::FastSin::Init();
	FastFFT<float>::Setup();
}

const int samplerate = 48000;
const int samplerateOversampled = 96000;
const int modUpdateRateOversampled = 16;
const int bufferSize = 64;
float lleft[bufferSize];
float rright[bufferSize];
float* buffer[2] = { lleft, rright };
uint8_t noteOnMessage[3] = { 0x90, 0x3C, 0x7F };

void FillWithNoise(float* buffer)
{
	AudioLib::LcgRandom rand(1);

	for (int i = 0; i < bufferSize; i++)
	{
		buffer[i] = rand.NextFloat() * 2 - 1;
	}
}

Synth* MakeSynth(int voiceCount)
{
	auto synth = new Polyhedrus::Synth();
	synth->Initialize(samplerate, true, 0, 0);
	synth->SetParameter(Synth::PackParameter(Module::Voices, (int)VoiceParameters::Polyphony), voiceCount);
	synth->SetParameter(Synth::PackParameter(Module::Voices, (int)VoiceParameters::Unison), voiceCount);
	synth->ProcessMidi(noteOnMessage);
	return synth;
}

void RunPerft1(int voiceCount, int seconds)
{
	auto synth = MakeSynth(voiceCount);

	int buffers = (seconds * samplerate) / bufferSize;
	auto freq = Polyhedrus::PlatformSpecific::PerformanceFrequency();
	auto start = Polyhedrus::PlatformSpecific::PerformanceCounter();

	for (int i = 0; i < buffers; i++)
	{
		synth->ProcessAudio(buffer, bufferSize);
	}

	auto end = Polyhedrus::PlatformSpecific::PerformanceCounter();
	auto totalTimeUs = (end - start) / (freq / 1000);

	std::cout << voiceCount << "," << seconds << "," << totalTimeUs << endl;

	delete synth;
}

void PerftCharacter(int seconds)
{
	FillWithNoise(lleft);

	Character character;
	character.Initialize(samplerateOversampled, bufferSize, modUpdateRateOversampled);
	character.Bottom = 0.5f;
	character.Top = 0.5f;
	character.Clip = 0.2f;
	character.Decimate = 0.2f;
	character.IsEnabled = true;
	character.Note = 60;
	character.Reduce = 0.5f;

	int buffers = (seconds * samplerate) / modUpdateRateOversampled; 
	auto freq = Polyhedrus::PlatformSpecific::PerformanceFrequency();
	auto start = Polyhedrus::PlatformSpecific::PerformanceCounter();

	for (int i = 0; i < buffers; i++)
	{
		character.Process(lleft, modUpdateRateOversampled);
	}

	auto end = Polyhedrus::PlatformSpecific::PerformanceCounter();
	auto totalTimeUs = (end - start) / (freq / 1000);

	std::cout << seconds << "," << totalTimeUs << endl;
}

void PerftFilterHp(int seconds)
{
	FillWithNoise(lleft);

	FilterHp filter;
	filter.Initialize(samplerateOversampled, bufferSize, modUpdateRateOversampled);
	filter.Cutoff = 0.2f;
	filter.IsEnabled = true;
	filter.Resonance = 0.1f;

	int buffers = (seconds * samplerate) / modUpdateRateOversampled;
	auto freq = Polyhedrus::PlatformSpecific::PerformanceFrequency();
	auto start = Polyhedrus::PlatformSpecific::PerformanceCounter();

	for (int i = 0; i < buffers; i++)
	{
		filter.Process(lleft, modUpdateRateOversampled);
	}

	auto end = Polyhedrus::PlatformSpecific::PerformanceCounter();
	auto totalTimeUs = (end - start) / (freq / 1000);

	std::cout << seconds << "," << totalTimeUs << endl;
}

void PerftNoise(int seconds)
{
	NoiseSimple noise;
	noise.Initialize(samplerateOversampled, bufferSize, 23);
	noise.Type = 0.75f;

	int buffers = (seconds * samplerate) / modUpdateRateOversampled;
	auto freq = Polyhedrus::PlatformSpecific::PerformanceFrequency();
	auto start = Polyhedrus::PlatformSpecific::PerformanceCounter();

	for (int i = 0; i < buffers; i++)
	{
		noise.Process(modUpdateRateOversampled);
	}

	auto end = Polyhedrus::PlatformSpecific::PerformanceCounter();
	auto totalTimeUs = (end - start) / (freq / 1000);

	std::cout << seconds << "," << totalTimeUs << endl;
}

void PerftDecimator(int seconds)
{
	FillWithNoise(lleft);
	AudioLib::Decimator deci;
	
	int buffers = (seconds * samplerate) / modUpdateRateOversampled;
	auto freq = Polyhedrus::PlatformSpecific::PerformanceFrequency();
	auto start = Polyhedrus::PlatformSpecific::PerformanceCounter();

	for (int i = 0; i < buffers; i++)
	{
		for (int j = 0; j < modUpdateRateOversampled; j+=2)
		{
			rright[j] = deci.Process(lleft[j], lleft[j + 1]);
			rright[j + 1] = rright[j];
		}
	}

	auto end = Polyhedrus::PlatformSpecific::PerformanceCounter();
	auto totalTimeUs = (end - start) / (freq / 1000);

	std::cout << seconds << "," << totalTimeUs << endl;
}

int main()
{
	Init();

	
	std::cout << "RunPerft1" << endl;
	cout << "Voices,Seconds,TimeMillis" << endl;
	while (true)
	{
		RunPerft1(1, 20);
		RunPerft1(20, 2);
	}
	

	/*
	std::cout << "PerftCharacter" << endl;
	cout << "Seconds,TimeMillis" << endl;
	while(true)
		PerftCharacter(500);
	*/

	/*
	std::cout << "PerftFilterHp" << endl;
	cout << "Seconds,TimeMillis" << endl;
	while (true)
		PerftFilterHp(500);
	*/

	/*
	std::cout << "PerfNoise" << endl;
	cout << "Seconds,TimeMillis" << endl;
	while (true)
		PerftNoise(500);
	*/

	/*
	std::cout << "PerftDecimator" << endl;
	cout << "Seconds,TimeMillis" << endl;
	while (true)
	PerftDecimator(500);
	*/

	int exit;
	std::cin >> exit;
	return 0;
}