#include "FilterDualSvf.h"
#include "AudioLib/ValueTables.h"
#include "AudioLib/Utils.h"
#include "AudioLib/Biquad.h"

namespace Polyhedrus
{
	FilterDualSvf::FilterDualSvf()
	{
		Drive = 0;
		Cutoff = 0;
		Resonance = 0;
		CutoffMod = 0;
		ResonanceMod = 0;
		DriveMod = 0;
		buffer = 0;
		gain = 1;
		gainInv = 1;
		totalResonance = 0;
		Mode = 0;
		ModeMod = 0;
		volLp = 1.0;
		volBp = 0.0;
		volHp = 0.0;
	}

	FilterDualSvf::~FilterDualSvf()
	{
		delete buffer;
	}

	void FilterDualSvf::Initialize(int samplerate, int bufferSize, int modulationUpdateRate)
	{
		buffer = new float[bufferSize]();
		this->modulationUpdateRate = modulationUpdateRate;
		this->samplerate = samplerate;
		fsinv = 1.0f / (2 * samplerate);
		cvToFreq.Initialize((float)samplerate);
		svf1.Fs = (float)(samplerate * 2); // internal oversample by 2x
		svf2.Fs = (float)(samplerate * 2);
		svf1.Nonlinear = true;
		svf2.Nonlinear = true;

		Cutoff = 1;
		Update();
	}

	void FilterDualSvf::Process(float* input, int len)
	{
		Update();

		for (int i = 0; i < len; i++)
		{
			float val = input[i] * gain;
			
			// oversample by 2x
			svf1.ProcessNonlinear(val);
			svf1.ProcessNonlinear(val);
			val = svf1.Lp * volLp + svf1.Bp * volBp + svf1.Hp * volHp;

			// oversample by 2x
			svf2.ProcessNonlinear(val);
			svf2.ProcessNonlinear(val);
			val = svf2.Lp * volLp + svf2.Bp * volBp + svf2.Hp * volHp;

			buffer[i] = val * gainInv;
		}
	}

	std::vector<float> FilterDualSvf::GetMagnitudeResponse()
	{
		float lpv = volLp;
		float bpv = volBp;
		float hpv = volHp;
		
		auto lp = AudioLib::Biquad::GetLowpassMagnitude(svf1.Fc, Resonance);
		auto bp = AudioLib::Biquad::GetBandpassMagnitude(svf1.Fc, Resonance);
		auto hp = AudioLib::Biquad::GetHighpassMagnitude(svf1.Fc, Resonance);

		std::vector<float> output;
		for (size_t i = 0; i < lp.size(); i++)
		{
			float val = lpv * lp[i] + bpv * bp[i] + hpv * hp[i];
			output.push_back(val);
		}

		return output;
	}

	void FilterDualSvf::Update()
	{
		float driveTotal = Drive + DriveMod;
		driveTotal = AudioLib::Utils::Limit(driveTotal, 0.0f, 1.0f);

		gain = (0.1f + 2.0f * driveTotal * driveTotal);
		gainInv = gain < 1.0 ? std::sqrt(1.0f / gain) : 1.0f;
		gainInv *= 1.3f; // gain fudge;

		float voltage = Cutoff + CutoffMod;
		voltage = AudioLib::Utils::Limit(voltage, 0.0f, 10.3f);
		float fc = cvToFreq.GetFreqWarped(voltage);

		totalResonance = Resonance + ResonanceMod;
		totalResonance = AudioLib::Utils::Limit(totalResonance, 0.0f, 1.0f);
		totalResonance = ((float)(1 - AudioLib::ValueTables::Get((1 - totalResonance), AudioLib::ValueTables::Response2Oct))) * 1.0f;

		svf1.Fc = fc;
		svf2.Fc = fc;
		svf1.Resonance = totalResonance;
		svf2.Resonance = totalResonance;
		svf1.Update();
		svf2.Update();

		float modeTotal = AudioLib::Utils::Limit(Mode + ModeMod, 0.0f, 1.0f) * 2;
		if (modeTotal <= 1.0)
		{
			volLp = 1.0f - modeTotal;
			volBp = modeTotal * 1.5f;
			volHp = 0.0f;
		}
		else
		{
			volLp = 0.0f;
			volBp = (2.0f - modeTotal) * 1.5f;
			volHp = modeTotal - 1.0f;
		}
	}
}

