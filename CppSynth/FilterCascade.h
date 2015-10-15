#ifndef LEIFTUR_FILTER_CASCADE
#define LEIFTUR_FILTER_CASCADE

namespace Leiftur
{
	class FilterCascade
	{
	public:
		const int Oversample = 4;

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

		float p;
		float x;
		float a;
		float b;
		float c;
		float d;
		float feedback;

		float fsinv;
		int samplerate;

	public:
		FilterCascade(int samplerate, int bufferSize);
		~FilterCascade();

		float ProcessSample(float input);
		void Process(float* input, int len);

	private:
		void Update();
	};
}

#endif

