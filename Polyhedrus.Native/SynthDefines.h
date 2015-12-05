#ifndef POLYHEDRUS_SYNTH_DEFINES
#define POLYHEDRUS_SYNTH_DEFINES

namespace Polyhedrus
{

	const int MaxVoiceCount = 24;
	const int ModulationUpdateRate = 8;
	const int BufferSize = 64;
	const int VoiceTuningCount = 20;

	enum class VoiceMode
	{
		MonoHighest = 0,
		MonoLowest = 1,
		MonoNewest = 2,
		PolyRoundRobin = 3,
	};

}

#endif