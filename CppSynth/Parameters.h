#ifndef LEIFTUR_PARAMETERS
#define LEIFTUR_PARAMETERS

#include <string>
#include <vector>
#include <cmath>

namespace Leiftur
{
	enum class Module
	{
		Control = 0,

		Osc1 = 1,
		Osc2 = 2,
		Osc3 = 3,

		Mixer = 10,
		Character = 15,

		FilterHp = 20,
		FilterMain = 21,
		Drive = 22,

		EnvAmp = 70,
		EnvFilter = 71,

		Mod1 = 81,
		Mod2 = 82,
		Mod3 = 83,

		Arp = 100,
		Voices = 110,

		Chorus = 150,
		Delay = 151,

		Macros = 190,
		ModMatrix = 200,
	};

	enum class OscParameters
	{
		Octave = 1,
		Semi = 2,
		Cent = 3,
		Pan = 4,
		Volume = 5,

		Slop = 6,
		Phase = 7,
		Shape = 8,

		Waveform = 9,
		Routing = 10 
		
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

	enum class CharacterParameters
	{
		Bottom = 1,
		Top = 2,
		Decimate = 3,
		Reduce = 4,
		Clip = 5,
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

	enum class DriveParameters
	{
		Gain = 1,
		Bias = 2,
		Post = 3,
		Type = 4,
		Mellow = 5,
	};

	enum class EnvParameters
	{
		Attack = 1,
		Hold = 2,
		Decay = 3,
		Sustain = 4,
		Release = 5,
		Velocity = 6,
		AttackCurve = 7,
		DecayCurve = 8,
		ReleaseCurve = 9,
		Retrigger = 10,
	};

	enum class ModParameters
	{
		Delay = 1,
		Attack = 2,
		Hold = 3,
		Decay = 4,
		Sustain = 5,
		Release = 6,

		Phase = 7,
		Freq = 8,
		Shape = 9,
		Slew = 10,
		Steps = 11,
		Sync = 12,

		AttackCurve = 13,
		DecayCurve = 14,
		ReleaseCurve = 15,
		Retrigger = 16,
	};

	enum class ArpParameters
	{
		Range = 1,
		NotePtn = 2,
		OctavePtn = 3,
		Gate = 4,
		Divide = 5,
		Bpm = 6,
		Sync = 7,
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

	enum class ChorusParameters
	{
		Enable1 = 1,
		Enable2 = 2,
		Rate1 = 3,
		Rate2 = 4,
		Depth1 = 5,
		Depth2 = 6,
		Width = 7,
		Quality = 8,
		Wet = 9,
	};

	enum class DelayParameters
	{
		DelayL = 1,
		DelayR  = 2,
		FeedbackL = 3,
		FeedbackR = 4,
		Lowpass = 5,
		Highpass = 6,
		Saturate = 7,
		Crossfeed = 8,
		Diffuse = 9,
		Wet = 10,
		Sync = 11,
	};

	enum class MacroParameters
	{
		Macro1 = 1,
		Macro2 = 2,
		Macro3 = 3,
		Macro4 = 4,
		Macro5 = 5,
		Macro6 = 6,
		Macro7 = 7,
		Macro8 = 8,
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
		static Module GetModule(std::string moduleString);
		static int GetParameter(std::string parameterString, Module module);
	public:
		static void ParseAddress(std::string address, Module* module, int* parameter);

		static int FloorToInt(double value)
		{
			auto sign = value < 0.0 ? -1 : 1;
			auto abs = sign == -1 ? -value : value;
			auto rounded = (int)(abs + 0.0001);
			return rounded * sign;
		}

		static int RoundToInt(double value)
		{
			auto sign = value < 0.0 ? -1 : 1;
			auto abs = sign == -1 ? -value : value;
			auto rounded = std::round(abs);
			int round2 = (int)(rounded + 0.0001);
			return round2 * sign;
		}
	};
}

#endif
