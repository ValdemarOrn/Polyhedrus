#include "FilterCascade.h"
#include "AudioLib/ValueTables.h"
#include "AudioLib/Utils.h"

Leiftur::FilterCascade::FilterCascade()
{
	Drive = 0.0;
	Cutoff = 1.0;
	Reso = 0.0;
	CutoffMod = 0.0;
	buffer = 0;
}

Leiftur::FilterCascade::~FilterCascade()
{
	delete buffer;
}

void Leiftur::FilterCascade::Initialize(int samplerate, int bufferSize, int modulationUpdateRate)
{
	buffer = new float[bufferSize];
	this->modulationUpdateRate = modulationUpdateRate;
	this->samplerate = samplerate;
	fsinv = 1.0 / (Oversample * samplerate);

	Cutoff = 1;
	VD = 1;
	updateCounter = 0;
	oversampledInput = 0;
	Update();
}

void Leiftur::FilterCascade::Process(float * input, int len)
{
	for (size_t i = 0; i < len; i++)
	{
		if (updateCounter <= 0)
		{
			Update();
			updateCounter = modulationUpdateRate;
		}

		float value = ProcessSample(input[i]);
		buffer[i] = value;
		updateCounter--;
	}
}

float * Leiftur::FilterCascade::GetOutput()
{
	return buffer;
}

float Leiftur::FilterCascade::ProcessSample(float input)
{
	float mx = 1.0 / Oversample;
	float sum = 0.0;

	for (int i = 0; i < Oversample; i++)
	{
		float in = mx * i * input + (Oversample - i) * mx * oversampledInput;
		in = AudioLib::Utils::TanhPoly(in * gain);

		float fb = Reso * 4 * (feedback - 0.5 * in);
		float val = in - fb;
		x = val;

		// 4 cascaded low pass stages
		a = (1 - p) * val + p * a;
		val = a;
		b = (1 - p) * val + p * b;
		val = b;
		c = (1 - p) * val + p * c;
		val = c;
		d = (1 - p) * val + p * d;
		val = d;

		feedback = AudioLib::Utils::TanhPoly(val);
		//auto sample = (VX * x + VA * a + VB * b + VC * c + VD * d) * (1 - Reso * 0.5);
		//sum += sample;
	}

	oversampledInput = input;
	//return sum * mx;
	auto sample = (VX * x + VA * a + VB * b + VC * c + VD * d) * (1 - Reso * 0.5);
	return sample;
}

void Leiftur::FilterCascade::Update()
{
	gain = (0.5 + 4.5 * Drive * Drive);

	if (Reso > 0.999)
		Reso = 0.999;
	else if (Reso < 0)
		Reso = 0;

	auto value = Cutoff + CutoffMod;

	if (value > 1)
		value = 1;
	else if (value < 0)
		value = 0;

	auto cutoff = 10 + AudioLib::ValueTables::Get(value, AudioLib::ValueTables::Response3Dec) * 21000;
	// Todo: get a proper lokup table to tune the filter
	p = (1 - 2 * cutoff * fsinv) * (1 - 2 * cutoff * fsinv);
}
