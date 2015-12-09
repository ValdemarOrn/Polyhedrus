#ifndef BIQUAD
#define BIQUAD

#include <vector>
using namespace std;

namespace AudioLib
{
	class Biquad
	{
	public:
		enum class FilterType
		{
			LowPass = 0,
			HighPass,
			BandPass,
			Notch,
			Peak,
			LowShelf,
			HighShelf
		};

	private:
		int samplerate;
		float _gainDb;
		float _q;
		float a0, a1, a2, b0, b1, b2;
		float x1, x2, y, y1, y2;
		float gain;

	public:
		FilterType Type;
		float Output;
		float Frequency;
		float Slope;

		Biquad();
		Biquad(FilterType filterType, int samplerate);
		~Biquad();

		int GetSamplerate();
		void SetSamplerate(int samplerate);
		float GetGainDb();
		void SetGainDb(float value);
		float GetGain();
		void SetGain(float value);
		float GetQ();
		void SetQ(float value);
		vector<float> GetA();
		vector<float> GetB();

		void Update();
		float GetResponse(float freq) const;
		
		float inline Process(float x)
		{
			y = b0 * x + b1 * x1 + b2 * x2 - a1 * y1 - a2 * y2;
			x2 = x1;
			y2 = y1;
			x1 = x;
			y1 = y;

			Output = y;
			return Output;
		}

		void inline Process(float* input, float* output, int len)
		{
			for (int i = 0; i < len; i++)
			{
				float x = input[i];
				y = ((b0 * x) + (b1 * x1) + (b2 * x2)) - (a1 * y1) - (a2 * y2);
				x2 = x1;
				y2 = y1;
				x1 = x;
				y1 = y;

				output[i] = y;
			}

			Output = y;
		}

		void ClearBuffers();

		static std::vector<float> GetLowpassMagnitude(float cutoff, float resonance);
		static std::vector<float> GetBandpassMagnitude(float cutoff, float resonance);
		static std::vector<float> GetHighpassMagnitude(float cutoff, float resonance);

	private:
		static std::vector<float> GetSystemResponse(const Biquad& biquad);
	};
}

#endif