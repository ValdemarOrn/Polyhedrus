#ifndef LEIFTUR_DRIVE
#define LEIFTUR_DRIVE

#include "Parameters.h"
#include "AudioLib/OnePoleFilters.h"
#include "AudioLib/Utils.h"
#include <cmath>

namespace Leiftur
{
	enum class DriveType
	{
		None = 0,
		Clip = 1,
		Tanh = 2,
		Asym = 3,
	};

	class Drive
	{
	public:
		float Gain;
		float Bias;
		bool Post;
		bool IsEnabled;
		DriveType Type;
		float Mellow;
		float Volume;

		float GainMod;
		float BiasMod;
		float MellowMod;

	private:
		float* buffer;
		int samplerate;
		int modulationUpdateRate;
		int updateCounter;
		AudioLib::Lp1 lp;
		AudioLib::Hp1 hp;
		float gainTotal;
		float biasTotal;

	public:
		Drive();
		~Drive();
		void Initialize(int samplerate, int bufferSize, int modulationUpdateRate);
		void SetParameter(DriveParameters parameter, double value);
		void Process(float* input, int len);
		inline float* GetOutput() { return buffer; }
	private:
		void Update();
		inline static float None(float x) { return x; }
		inline static float Asym(float x) { return x <= -1 ? -1.0f : (x > 2.0f ? AsymCurve[299999] : AsymCurve[(int)((x + 1) * 100000)]); }
		inline static float Clip(float x) { return AudioLib::Utils::Limit(x, -1.0f, 1.0f); }
		inline static float Tanh(float x) { return AudioLib::Utils::TanhLookup(x); }

		static float AsymCurve[300000];
		static void InitCurves();
	};
}

#endif