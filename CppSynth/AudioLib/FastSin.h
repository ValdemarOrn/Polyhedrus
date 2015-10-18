#ifndef FASTSIN
#define FASTSIN

namespace AudioLib
{
	class FastSin
	{
	private:
		static const int DataSize = 32768;
		static double data[DataSize];

	public:
		static void Init();
		static double Get(double index);
	};
}

#endif
