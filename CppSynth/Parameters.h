#ifndef LEIFTUR_PARAMETERS
#define LEIFTUR_PARAMETERS

#include <string>
#include <vector>

namespace Leiftur
{
	enum class Module
	{
		Osc1 = 1,
		Osc2 = 2,
		Osc3 = 3,

		Mixer = 10,

		FilterHp = 20,
		FilterMain = 21,

		EnvAmp = 70,
		EnvFilter = 71,
		EnvMod = 72,

		Lfo1 = 81,
		Lfo2 = 82,

		Arp = 100,
		Voices = 110,

		ModMatrix = 200,
	};

	enum class OscParameters
	{
		Slop = 1,
		Phase = 2,
		Octave = 3,
		Semi = 4,
		Cent = 5,
		Waveform = 6,
		Shape = 7,
		Pan = 8,
		Volume = 9
	};

	enum class MixerParameters
	{
		Am12 = 1,
		Am23 = 2,
		Fm12 = 3,
		Fm13 = 4,
		Fm23 = 5,
		Noise = 6,
		Color = 7,
		Output = 8,
	};

	enum class FilterHpParameters
	{
		Cutoff = 1,
		Resonance = 2,
		Keytrack = 3,
		Env = 4,
	};

	enum class FilterMainParameters
	{
		Drive = 1,
		Cutoff = 2,
		Resonance = 3,
		Keytrack = 4,
		Env = 5,
		Type = 6,
	};

	enum class EnvParameters
	{
		Attack = 1,
		Hold = 2,
		Decay = 3,
		Sustain = 4,
		Release = 5,
		Velocity = 6,
		Retrigger = 7,
	};

	enum class LfoParameters
	{
		Phase = 1,
		Freq = 2,
		Shape = 3,
		Attack = 4,
		Decay = 5,
		Bipolar = 6,
		Sync = 7,
	};

	enum class ArpParameters
	{
		Range = 1,
		NotePtn = 2,
		OctavePtn = 3,
		Gate = 4,
		Tempo = 5,
		Sync = 6,
	};

	enum class VoiceParameters
	{
		Detune = 1,
		Spread = 2,
		Glide = 3,
		Bend = 4,
		Master = 5,
		Polyphony = 6,
		Unison = 7,
		VoiceMode = 8,
	};

	enum class ModMatrixParameters
	{
		Source1     = 10,
		Dest1       = 11,
		Via1        = 12,
		Amount1     = 13,
		ViaAmount1  = 14,

		Source2     = 20,
		Dest2       = 21,
		Via2        = 22,
		Amount2     = 23,
		ViaAmount2  = 24,

		Source3     = 30,
		Dest3       = 31,
		Via3        = 32,
		Amount3     = 33,
		ViaAmount3  = 34,

		Source4     = 40,
		Dest4       = 41,
		Via4        = 42,
		Amount4     = 43,
		ViaAmount4  = 44,

		Source5     = 50,
		Dest5       = 51,
		Via5        = 52,
		Amount5     = 53,
		ViaAmount5  = 54,

		Source6     = 60,
		Dest6       = 61,
		Via6        = 62,
		Amount6     = 63,
		ViaAmount6  = 64,

		Source7     = 70,
		Dest7       = 71,
		Via7        = 72,
		Amount7     = 73,
		ViaAmount7  = 74,

		Source8     = 80,
		Dest8       = 81,
		Via8        = 82,
		Amount8     = 83,
		ViaAmount8  = 84,

		Source9     = 90,
		Dest9       = 91,
		Via9        = 92,
		Amount9     = 93,
		ViaAmount9  = 94,

		Source10    = 100,
		Dest10      = 101,
		Via10       = 102,
		Amount10    = 103,
		ViaAmount10 = 104,

		Source11    = 110,
		Dest11      = 111,
		Via11       = 112,
		Amount11    = 113,
		ViaAmount11 = 114,

		Source12    = 120,
		Dest12      = 121,
		Via12       = 122,
		Amount12    = 123,
		ViaAmount12 = 124,

		Source13    = 130,
		Dest13      = 131,
		Via13       = 132,
		Amount13    = 133,
		ViaAmount13 = 134,

		Source14    = 140,
		Dest14      = 141,
		Via14       = 142,
		Amount14    = 143,
		ViaAmount14 = 144,

		Source15    = 150,
		Dest15      = 151,
		Via15       = 152,
		Amount15    = 153,
		ViaAmount15 = 154,

		Source16    = 160,
		Dest16      = 161,
		Via16       = 162,
		Amount16    = 163,
		ViaAmount16 = 164,
	};

	class Parameters
	{
	private:
		static std::vector<std::string> Split(const std::string s, char delim);
		static Module GetModule(std::string moduleString);
		static int GetParameter(std::string parameterString, Module module);
	public:
		static void ParseAddress(std::string address, Module* module, int* parameter);
	};
}

#endif
