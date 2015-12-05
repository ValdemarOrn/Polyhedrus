#ifndef LEIFTUR_PARAMETERS
#define LEIFTUR_PARAMETERS

#include <string>
#include <vector>
#include <cmath>
#include <functional>
#include <map>

#include "Utils.h"

namespace Leiftur
{
	enum class Module
	{
		Control = 0,

		Osc1 = 1,
		Osc2 = 2,
		Osc3 = 3,

		Mixer = 10,
		ModuleSwitches = 11,
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
		Linear = 1,
		Octave = 2,
		Semi = 3,
		Cent = 4,
		Pan = 5,
		Volume = 6,

		Keytrack = 7,
		Slop = 8,
		Phase = 9,
		Shape = 10,

		Waveform = 11,
		Routing = 12 
		
	};

	enum class MixerParameters
	{
		Am12 = 1,
		Am23 = 2,
		Fm12 = 3,
		Fm13 = 4,
		Fm23 = 5,
		Color = 6,

		CharacterOut = 7,
		FilterHpOut = 8,
		FilterMainOut = 9,

		NoiseRouting = 10,
		Noise = 11,
	};

	enum class ModuleSwitchParameters
	{
		Osc1On = 1,
		Osc2On = 2,
		Osc3On = 3,
		CharacterOn = 4,
		FilterHpOn = 5,
		FilterMainOn = 6,
		DriveOn = 7,
		Mod1On = 8,
		Mod2On = 9,
		Mod3On = 10,
		ArpOn = 11,
		ChorusOn = 12,
		DelayOn = 13,
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
		Mode = 6,
		Type = 7,
	};

	enum class DriveParameters
	{
		Gain = 1,
		Bias = 2,
		Volume = 3,
		Post = 4,
		Type = 5,
		Mellow = 6,
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
		VelocityCurve = 10,
		Retrigger = 11,
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
		VelocityCurve = 16,
		Velocity = 17,
		Retrigger = 18,
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
		HiQuality = 6,
		Polyphony = 7,
		Unison = 8,
		VoiceMode = 9,
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
		DiffuseAmount = 8,
		DiffuseSize = 9,
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
		Source_1     = 10,
		Dest_1       = 11,
		Via_1        = 12,
		Amount_1     = 13,
		ViaAmount_1  = 14,

		Source_2     = 20,
		Dest_2       = 21,
		Via_2        = 22,
		Amount_2     = 23,
		ViaAmount_2  = 24,

		Source_3     = 30,
		Dest_3       = 31,
		Via_3        = 32,
		Amount_3     = 33,
		ViaAmount_3  = 34,

		Source_4     = 40,
		Dest_4       = 41,
		Via_4        = 42,
		Amount_4     = 43,
		ViaAmount_4  = 44,

		Source_5     = 50,
		Dest_5       = 51,
		Via_5        = 52,
		Amount_5     = 53,
		ViaAmount_5  = 54,

		Source_6     = 60,
		Dest_6       = 61,
		Via_6        = 62,
		Amount_6     = 63,
		ViaAmount_6  = 64,

		Source_7     = 70,
		Dest_7       = 71,
		Via_7        = 72,
		Amount_7     = 73,
		ViaAmount_7  = 74,

		Source_8     = 80,
		Dest_8       = 81,
		Via_8        = 82,
		Amount_8     = 83,
		ViaAmount_8  = 84,

		Source_9     = 90,
		Dest_9       = 91,
		Via_9        = 92,
		Amount_9     = 93,
		ViaAmount_9  = 94,

		Source_10    = 100,
		Dest_10      = 101,
		Via_10       = 102,
		Amount_10    = 103,
		ViaAmount_10 = 104,

		Source_11    = 110,
		Dest_11      = 111,
		Via_11       = 112,
		Amount_11    = 113,
		ViaAmount_11 = 114,

		Source_12    = 120,
		Dest_12      = 121,
		Via_12       = 122,
		Amount_12    = 123,
		ViaAmount_12 = 124,

		Source_13    = 130,
		Dest_13      = 131,
		Via_13       = 132,
		Amount_13    = 133,
		ViaAmount_13 = 134,

		Source_14    = 140,
		Dest_14      = 141,
		Via_14       = 142,
		Amount_14    = 143,
		ViaAmount_14 = 144,

		Source_15    = 150,
		Dest_15      = 151,
		Via_15       = 152,
		Amount_15    = 153,
		ViaAmount_15 = 154,

		Source_16    = 160,
		Dest_16      = 161,
		Via_16       = 162,
		Amount_16    = 163,
		ViaAmount_16 = 164,
	};

	struct ParameterInfo
	{
		int ParameterIndex;
		std::string ParameterName;
		std::string FormattedName;
		double DefaultValue;
		double MinValue;
		double MaxValue;
		std::function<std::string(double)> Formatter;

		inline ParameterInfo()
		{
			ParameterIndex = 0;
			ParameterName = "";
			FormattedName = "";
			DefaultValue = 0.0;
			MinValue = 0.0;
			MaxValue = 1.0;
			Formatter = [](double val) -> std::string { return SPrint("%.3f", val); };
		}

		inline ParameterInfo(int parameterIndex, const char* parameterName, const char* formattedName, double defaultValue, double minValue, double maxValue, std::function<std::string(double)> formatter)
		{
			this->ParameterIndex = parameterIndex;
			this->ParameterName = std::string(parameterName);
			this->FormattedName = formattedName == 0 ? std::string(parameterName) : std::string(formattedName);
			this->DefaultValue = defaultValue;
			this->MinValue = minValue;
			this->MaxValue = maxValue;
			this->Formatter = formatter == 0
				? [](double val) -> std::string { return SPrint("%.3f", val); }
				: formatter;
		}
	};

	class Parameters
	{
	public:
		static std::map<Module, std::map<int, ParameterInfo>> ParamInfo;
		static void Init();

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
