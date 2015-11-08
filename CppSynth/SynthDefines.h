#ifndef LEIFTUR_SYNTH_DEFINES
#define LEIFTUR_SYNTH_DEFINES

namespace Leiftur
{

	const int MaxVoiceCount = 32;
	const int ModulationUpdateRate = 8;
	const int BufferSize = 64;

	enum class VoiceMode
	{
		MonoHighest = 0,
		MonoLowest = 1,
		MonoNewest = 2,
		PolyRoundRobin = 3,
	};

}

#endif