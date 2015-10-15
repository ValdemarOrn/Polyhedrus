#include "FilterCascade.h"
#include "AudioLib/ValueTables.h"
#include "AudioLib/Utils.h"

Leiftur::FilterCascade::FilterCascade(int samplerate, int bufferSize)
{
	buffer = new float[bufferSize];

	this->samplerate = samplerate;
	fsinv = 1.0 / (Oversample * samplerate);

	Cutoff = 1;
	VD = 1;
	Update();
}

Leiftur::FilterCascade::~FilterCascade()
{
	delete buffer;
}

float Leiftur::FilterCascade::ProcessSample(float input)
{
	for (int i = 0; i < Oversample; i++)
	{
		float fb = Reso * 5 * (feedback - 0.5 * input);
		float val = input - fb;
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
	}

	auto sample = (VX * x + VA * a + VB * b + VC * c + VD * d) * (1 - Reso * 0.5);
	return sample;
}

void Leiftur::FilterCascade::Process(float * input, int len)
{
	for (size_t i = 0; i < len; i++)
	{
		float value = ProcessSample(input[i]);
	}
}

void Leiftur::FilterCascade::Update()
{
	if (Reso > 1)
		Reso = 1;
	else if (Reso < 0)
		Reso = 0;

	auto value = Cutoff + CutoffMod;

	if (value > 1)
		value = 1;
	else if (value < 0)
		value = 0;

	auto cutoff = 10 + AudioLib::ValueTables::Get(value, AudioLib::ValueTables::Response3Dec) * 21000;
	p = (1 - 2 * cutoff * fsinv) * (1 - 2 * cutoff * fsinv);
}
