#ifndef LEIFTUR_FILTER_MAIN
#define LEIFTUR_FILTER_MAIN

namespace Leiftur
{
	class FilterMain
	{
	public:
		float Drive;
		float Cutoff;
		float Reso;
		int Key;
		float PitchBend;
		float CutoffMod;
		int Type;

	private:
		float* buffer;

	public:
		FilterMain(int samplerate, int bufferSize);
		~FilterMain();

		void Process(float* input, int len);

	private:
		void Update();
	};
}

#endif

