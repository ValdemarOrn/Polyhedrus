#include "FilterMain.h"

namespace Leiftur
{
	FilterMain::FilterMain(int samplerate, int bufferSize)
	{
		buffer = new float[bufferSize];
	}

	FilterMain::~FilterMain()
	{
		delete buffer;
	}

}