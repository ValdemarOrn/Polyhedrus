#include "OscTests.h"

extern "C"
{
	_declspec(dllexport) void RunTests()
	{
		Tests::Osc::TestOscParser1();
		Tests::Osc::TestOscParserFloatInt();
		Tests::Osc::TestOscParserStringBlobInt();
		Tests::Osc::TestBundleParse();
	}
}