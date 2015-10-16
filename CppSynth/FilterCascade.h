#ifndef LEIFTUR_FILTER_CASCADE
#define LEIFTUR_FILTER_CASCADE

namespace Leiftur
{
	class FilterCascade
	{
	public:
		const int Oversample = 4;

		float Drive;
		float Cutoff;
		float Reso;
		float CutoffMod;

		float VA;
		float VB;
		float VC;
		float VD;
		float VX;

	private:
		float* buffer;
		float gain;
		float oversampledInput;

		float p;
		float x;
		float a;
		float b;
		float c;
		float d;
		float feedback;

		float fsinv;
		int samplerate;
		int updateCounter;
		int modulationUpdateRate;

	public:
		FilterCascade();
		~FilterCascade();

		void Initialize(int samplerate, int bufferSize, int modulationUpdateRate);
		void Process(float* input, int len);
		float* GetOutput();

	private:
		float ProcessSample(float input);
		void Update();
	};
}

#endif

