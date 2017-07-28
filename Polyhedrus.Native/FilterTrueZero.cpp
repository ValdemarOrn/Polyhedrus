#include "Default.h"
#include "FilterTrueZero.h"
#include "AudioLib/ValueTables.h"
#include "AudioLib/Utils.h"
#include "AudioLib/MathDefs.h"
#include "AudioLib/Biquad.h"

namespace Polyhedrus
{
	const float M_PI_INV = (float)(1.0 / M_PI);
	double iterFilter = 0.0; // just for record keeping during debugging

	float FilterTrueZero::CVtoAlpha[CVtoAlphaSize];

	void FilterTrueZero::ComputeCVtoAlpha(int samplerate)
	{
		double T = 1.0 / samplerate;
		for (int i = 0; i < CVtoAlphaSize; i++)
		{
			double freq = 440.0 * std::pow(2, (i*0.01 - 69.0 + 12) / 12);
			CVtoAlpha[i] = (float)((2 * M_PI * T * freq) * (2 * M_PI * T * freq + 1));
		}
	}

	FilterTrueZero::FilterTrueZero()
	{
		gain = 0;
		gainInv = 0;
		totalResonance = 0;
		fsinv = 0;
		samplerate = 0;
		bufferSize = 0;
		modulationUpdateRate = 0;

		Drive = 0.0;
		Cutoff = 1.0;
		Resonance = 0.0;
		ResonanceMod = 0.0;
		CutoffMod = 0.0;
		DriveMod = 0.0;
		buffer = 0;
		stage4Output = 0;
		convergenceRate = 0.5;
	}

	FilterTrueZero::~FilterTrueZero()
	{
		delete[] buffer;
	}

	FilterMainType FilterTrueZero::GetType() const
	{
		return FilterMainType::TrueZero;
	}

	void FilterTrueZero::Initialize(int samplerate, int bufferSize, int modulationUpdateRate)
	{
		ComputeCVtoAlpha(samplerate);
		buffer = new float[bufferSize]();
		this->bufferSize = bufferSize;
		this->modulationUpdateRate = modulationUpdateRate;
		this->samplerate = samplerate;
		fsinv = 1.0f / samplerate;
		CvToFreq.Initialize((float)samplerate);

		Cutoff = 1;
		Update();
	}

	void FilterTrueZero::SetParameter(FilterMainParameters parameter, double value)
	{
		switch (parameter)
		{
		case FilterMainParameters::Cutoff:
			Cutoff = (float)value;
			break;
		case FilterMainParameters::Drive:
			Drive = (float)value;
			break;
		case FilterMainParameters::Resonance:
			Resonance = (float)value;
			break;
		case FilterMainParameters::Mode:
			// todo
			break;
		}
	}

	void FilterTrueZero::Process(float* input, int len)
	{
		Update();

		for (int i = 0; i < len; i++)
		{
			float x = input[i] * gain;
			ProcessSample(x);
			float value = stage4Output * gainInv;
			buffer[i] = value;
		}
	}

	float* FilterTrueZero::GetOutput()
	{
		return buffer;
	}

	std::vector<uint8_t> FilterTrueZero::GetVisual()
	{
		auto transform = [](vector<float> input) -> vector<uint8_t>
		{
			vector<uint8_t> output;
			float min = -60;
			float max = 80;
			for (size_t i = 0; i < input.size(); i++)
			{
				float db = (float)AudioLib::Utils::Gain2DB(input[i]);
				if (db < min) db = min;
				if (db > max) db = max;

				int val = (int)((db - min) * 1.8f);
				output.push_back(val);
			}

			return output;
		};
		
		auto cutoff = CvToFreq.GetFreqWarped(AudioLib::Utils::Limit((float)Cutoff, 0.0f, 10.3f));
		auto vv = AudioLib::Biquad::GetLowpassMagnitude(cutoff, Resonance);
		return transform(vv);
	}

	std::vector<uint8_t> FilterTrueZero::GetDriveVisual()
	{
		std::vector<uint8_t> output;
		std::vector<float> floatOutput;
		float gain = (0.2f + 1.8f * Drive * Drive);
		float min = 9999.0f;
		float max = -9999.0f;

		for (size_t i = 0; i < 256; i++)
		{
			float value = (float)((-1 + 2 * i / 255.0) * gain);
			value = std::tanh(value);
			if (value < min) min = value;
			if (value > max) max = value;
			floatOutput.push_back(value);
		}

		float scaler = 1.0f / (max - min) * 255.99f;

		for (size_t i = 0; i < floatOutput.size(); i++)
		{
			output.push_back((uint8_t)((floatOutput[i] - min) * scaler));
		}

		return output;
	}

	std::string FilterTrueZero::GetModeString()
	{
		return SPrint("%.2f", Mode);
	}

	void FilterTrueZero::ProcessSample(float x)
	{
		float fb = stage4Output;
		int iters = 0;
		float diff = 1;

		// iterative solution of the fb variable
		while (true)
		{
			iters++;
			float val = x - fb * 5 * totalResonance;
			val = AudioLib::Utils::TanhPoly(val);

			// 4 cascaded low pass stages
			val = stage1.ProcessNoUpdate(val);
			val = stage2.ProcessNoUpdate(val);
			val = stage3.ProcessNoUpdate(val);
			val = stage4.ProcessNoUpdate(val);

			float newFb = val;
			if (abs(newFb - fb) < 0.000001 && diff < 0.000001)
			{
				fb = newFb;
				break;
			}
			else
			{
				float newDiff = newFb - fb;
				if (newDiff * diff > 0) // check if newDiff and the old diff have the same sign, i.e. if the feedback is oscillating. we don't want that, it slows down convergence
				{
					convergenceRate = convergenceRate * 0.9f;
					if (convergenceRate < 0.005f)
						convergenceRate = 0.005f;
				}
				else
				{
					convergenceRate = convergenceRate * 1.1f;
					if (convergenceRate > 0.8f)
						convergenceRate = 0.8f;
				}

				diff = newDiff;
				fb = fb * convergenceRate + newFb * (1 - convergenceRate);
			}

			if (iters > 50)
			{ 
				fb = stage4Output;
				convergenceRate = 0.5;
				break;
			}
		} 

		iterFilter = iterFilter * 0.999 + iters * 0.001;

		float val = x - fb * 5 * totalResonance;
		val = AudioLib::Utils::TanhPoly(val);

		// 4 cascaded low pass stages
		val = stage1.Process(val);
		val = stage2.Process(val);
		val = stage3.Process(val);
		val = stage4.Process(val);
		stage4Output = val;
	}

	void FilterTrueZero::Update()
	{
		float driveTotal = Drive + DriveMod;
		driveTotal = AudioLib::Utils::Limit(driveTotal, 0.0f, 1.0f);

		gain = (0.2f + 1.8f * driveTotal * driveTotal);
		gainInv = gain < 1.0 ? std::sqrt(1.0f / gain) : 1 / gain;

		totalResonance = Resonance + ResonanceMod;
		totalResonance = AudioLib::Utils::Limit(totalResonance, 0.0f, 1.0f);
		totalResonance = (float)AudioLib::ValueTables::Get(totalResonance, AudioLib::ValueTables::Response2Oct) * 0.999f;
		
		// Voltage is 1V/OCt, C0 = 16.3516Hz
		float voltage = Cutoff + CutoffMod;
		voltage = AudioLib::Utils::Limit(voltage, 0.0f, 10.3f);
		float fc = CvToFreq.GetFreqWarped(voltage);

		float omega_c = (float)(fc * 2.0 * M_PI);
		
		//float omega_warped = 2 / T * tan(omega_c * T / 2);
		float omega_warped = 2.0f * samplerate * atan(omega_c * fsinv * 0.5f); // re-write of line above
		float fc_warped = omega_warped * 0.5f * M_PI_INV;
		float fac = fc_warped * fsinv;
		stage1.SetFc(fac);
		stage2.SetFc(fac);
		stage3.SetFc(fac);
		stage4.SetFc(fac);
	}

}
