#ifndef POLYHEDRUS_VCA
#define POLYHEDRUS_VCA

namespace Polyhedrus
{
	class Vca
	{
	public:
		float ControlVoltage;
	private:
		float* buffer;
		int samplerate;
		int modulationUpdateRate;
		float currentCv;

	public:
		Vca();
		~Vca();
		void Initialize(int samplerate, int bufferSize, int modulationUpdateRate);
		void Process(float* input, int len);
		float* GetOutput();
	private:
		void Update();
	};
}

#endif

