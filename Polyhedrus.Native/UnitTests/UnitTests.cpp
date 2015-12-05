#include "OscTests.h"
#include "WaveTests.h"

extern "C"
{
	_declspec(dllexport) void RunTests()
	{
		Tests::Wave::TestWaveReader1();

		Tests::Osc::TestOscParser1();
		Tests::Osc::TestOscParserFloatInt();
		Tests::Osc::TestOscParserStringBlobInt();
		Tests::Osc::TestBundleParse();
	}
}