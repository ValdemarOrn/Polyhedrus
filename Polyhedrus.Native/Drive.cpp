#include "Drive.h"
#include "AudioLib/Utils.h"
#include "AudioLib/ValueTables.h"

using namespace AudioLib;

namespace Polyhedrus
{

	Drive::Drive()
	{
		InitCurves();

		Gain = 0;
		Bias = 0;
		Post = true;
		Type = DriveType::None;
		Mellow = 0;
		buffer = 0;
		samplerate = 48000;
		IsEnabled = true;
	}

	Drive::~Drive()
	{
		delete buffer;
	}

	void Drive::Initialize(int samplerate, int bufferSize)
	{
		buffer = new float[bufferSize]();
		this->samplerate = samplerate;
		lp.SetFc(1);
		hp.SetFc(5.0f / 48000.0f);
	}

	void Drive::SetParameter(DriveParameters parameter, double value)
	{
		switch (parameter)
		{
		case DriveParameters::Gain:
			Gain = (float)value;
			break;
		case DriveParameters::Bias:
			Bias = (float)value;
			break;
		case DriveParameters::Volume:
			Volume = (float)value;
			break;
		case DriveParameters::Post:
			Post = value >= 0.5;
			break;
		case DriveParameters::Type:
			Type = (DriveType)Parameters::FloorToInt(value);
			if (value < 0 || value >= ((int)DriveType::Asym + 1))
				Type = DriveType::None;
			break;
		case DriveParameters::Mellow:
			Mellow = (float)value;
			break;
		}
	}

	void Drive::Process(float* input, int len)
	{
		if (!IsEnabled)
		{
			Utils::Copy(input, buffer, len);
			return;
		}

		auto func = &None;

		if (Type == DriveType::Asym)
			func = &Asym;
		else if (Type == DriveType::Clip)
			func = &Clip;
		else if (Type == DriveType::Tanh)
			func = &Tanh;

		Update();

		for (int i = 0; i < len; i++)
		{
			buffer[i] = input[i] * gainTotal + biasTotal;
		}

		for (int i = 0; i < len; i++)
		{
			buffer[i] = func(buffer[i]);
		}

		for (int i = 0; i < len; i++)
		{
			buffer[i] = lp.Process(buffer[i]);
		}

		for (int i = 0; i < len; i++)
		{
			buffer[i] = hp.Process(buffer[i]);
		}

		Utils::Gain(buffer, Volume, len);
	}

	void Drive::Update()
	{
		if (Type == DriveType::None)
		{
			gainTotal = 1.0;
			biasTotal = 0.0;
		}
		else
		{
			gainTotal = 1.0f + Utils::Limit(Gain + GainMod, 0.0, 1.0) * 20.0f;
			biasTotal = (Bias + BiasMod) * gainTotal;
		}

		float fc = Utils::Limit(1 - (Mellow + MellowMod), 0.0, 1.0);
		fc = 0.002f + (float)ValueTables::Get(fc, ValueTables::Response2Dec);
		lp.SetFc(fc);
	}

	float Drive::AsymCurve[300000];
	bool Drive::curvesInitialized = false;

	void Drive::InitCurves()
	{
		if (curvesInitialized)
			return;

		for (size_t i = 0; i < 300000; i++)
		{
			float x = (float)(-1 + i / 100000.0);
			AsymCurve[i] = (float)(2 * std::tanh(0.2f * std::powf(2 * x + 2, 1.5)) - 1);
		}

		curvesInitialized = true;
	}
}

