using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Leiftur.Ui
{
	enum Module
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
	}

	enum OscParameters
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
	}

	enum MixerParameters
	{
		Am12 = 1,
		Am23 = 2,
		Fm12 = 3,
		Fm13 = 4,
		Fm23 = 5,
		Noise = 6,
		Color = 7,
		Output = 8,
		NoiseRouting = 9,
	}

	enum ModuleSwitchParameters
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
	}

	enum CharacterParameters
	{
		Bottom = 1,
		Top = 2,
		Decimate = 3,
		Reduce = 4,
		Clip = 5,
	}

	enum FilterHpParameters
	{
		Cutoff = 1,
		Resonance = 2,
		Keytrack = 3,
		Env = 4,
	}

	enum FilterMainParameters
	{
		Drive = 1,
		Cutoff = 2,
		Resonance = 3,
		Keytrack = 4,
		Env = 5,
		Type = 6,
	}

	enum DriveParameters
	{
		Gain = 1,
		Bias = 2,
		Post = 3,
		Type = 4,
		Mellow = 5,
	}

	enum EnvParameters
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
	}

	enum ModParameters
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
	}

	enum ArpParameters
	{
		Range = 1,
		NotePtn = 2,
		OctavePtn = 3,
		Gate = 4,
		Divide = 5,
		Bpm = 6,
		Sync = 7,
	}

	enum VoiceParameters
	{
		Detune = 1,
		Spread = 2,
		Glide = 3,
		Bend = 4,
		Master = 5,
		Polyphony = 6,
		Unison = 7,
		VoiceMode = 8,
	}

	enum ChorusParameters
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
	}

	enum DelayParameters
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
	}

	enum MacroParameters
	{
		Macro1 = 1,
		Macro2 = 2,
		Macro3 = 3,
		Macro4 = 4,
		Macro5 = 5,
		Macro6 = 6,
		Macro7 = 7,
		Macro8 = 8,
	}

	enum ModMatrixParameters
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
	}

	static class Parameters
	{
		public static Tuple<Module, int> Unpack(int packedParameter)
		{
			var module = (packedParameter & 0xFFFF0000) >> 16;
			var parameter = (packedParameter & 0x0000FFFF);
			return Tuple.Create((Module)module, parameter);
		}

		public static int Pack(Module module, int parameter)
		{
			return (((int)module) << 16) | parameter;
		}

		public static int Pack(string address)
		{
			var tuple = ParseAddress(address);
			return Pack(tuple.Item1, tuple.Item2);
		}

		public static Tuple<Module, int> ParseAddress(string address)
		{
			var parts = address.Split(new[] { '/' }, StringSplitOptions.RemoveEmptyEntries);
			Module module;
			var ok = Enum.TryParse<Module>(parts[0], true, out module);
			if (!ok)
				return Tuple.Create(Module.Control, -1);

			int parameter = -1;

			switch (module)
			{
				case Module.Osc1:
				case Module.Osc2:
				case Module.Osc3:
					parameter = (int)(OscParameters)Enum.Parse(typeof(OscParameters), parts[1], true);
					break;
				case Module.Mixer:
					parameter = (int)(MixerParameters)Enum.Parse(typeof(MixerParameters), parts[1], true);
					break;
				case Module.ModuleSwitches:
					parameter = (int)(ModuleSwitchParameters)Enum.Parse(typeof(ModuleSwitchParameters), parts[1], true);
					break;
				case Module.Character:
					parameter = (int)(CharacterParameters)Enum.Parse(typeof(CharacterParameters), parts[1], true);
					break;
				case Module.FilterHp:
					parameter = (int)(FilterHpParameters)Enum.Parse(typeof(FilterHpParameters), parts[1], true);
					break;
				case Module.FilterMain:
					parameter = (int)(FilterMainParameters)Enum.Parse(typeof(FilterMainParameters), parts[1], true);
					break;
				case Module.Drive:
					parameter = (int)(DriveParameters)Enum.Parse(typeof(DriveParameters), parts[1], true);
					break;
				case Module.EnvAmp:
				case Module.EnvFilter:
					parameter = (int)(EnvParameters)Enum.Parse(typeof(EnvParameters), parts[1], true);
					break;
				case Module.Mod1:
				case Module.Mod2:
				case Module.Mod3:
					parameter = (int)(ModParameters)Enum.Parse(typeof(ModParameters), parts[1], true);
					break;
				case Module.Arp:
					parameter = (int)(ArpParameters)Enum.Parse(typeof(ArpParameters), parts[1], true);
					break;
				case Module.Voices:
					parameter = (int)(VoiceParameters)Enum.Parse(typeof(VoiceParameters), parts[1], true);
					break;
				case Module.Chorus:
					parameter = (int)(ChorusParameters)Enum.Parse(typeof(ChorusParameters), parts[1], true);
					break;
				case Module.Delay:
					parameter = (int)(DelayParameters)Enum.Parse(typeof(DelayParameters), parts[1], true);
					break;
				case Module.Macros:
					parameter = (int)(MacroParameters)Enum.Parse(typeof(MacroParameters), parts[1], true);
					break;
				case Module.ModMatrix:
					parameter = (int)(ModMatrixParameters)Enum.Parse(typeof(ModMatrixParameters), parts[1], true);
					break;
			}

			return Tuple.Create(module, parameter);
		}

		public static string PrettyPrint(Module module, int parameter)
		{
			Func<string, string> Pretty = (str) =>
			{
				var output = new List<char>();

				foreach (var ch in str)
				{
					if (char.IsUpper(ch))
						output.Add(' ');

					output.Add(ch);
				}

				return new string(output.ToArray()).Trim();
			};

			switch (module)
			{
				case Module.Osc1:
				case Module.Osc2:
				case Module.Osc3:
					return module + " " + Pretty(((OscParameters)parameter).ToString());
				case Module.Mixer:
					return module + " " + Pretty(((MixerParameters)parameter).ToString());
				case Module.Character:
					return module + " " + Pretty(((CharacterParameters)parameter).ToString());
				case Module.FilterHp:
					return module + " " + Pretty(((FilterHpParameters)parameter).ToString());
				case Module.FilterMain:
					return module + " " + Pretty(((FilterMainParameters)parameter).ToString());
				case Module.Drive:
					return module + " " + Pretty(((DriveParameters)parameter).ToString());
				case Module.EnvAmp:
				case Module.EnvFilter:
					return module + " " + Pretty(((EnvParameters)parameter).ToString());
				case Module.Mod1:
				case Module.Mod2:
				case Module.Mod3:
					return module + " " + Pretty(((ModParameters)parameter).ToString());
				case Module.Arp:
					return module + " " + Pretty(((ArpParameters)parameter).ToString());
				case Module.Voices:
					return module + " " + Pretty(((VoiceParameters)parameter).ToString());
				case Module.Chorus:
					return module + " " + Pretty(((ChorusParameters)parameter).ToString());
				case Module.Delay:
					return module + " " + Pretty(((DelayParameters)parameter).ToString());
				case Module.Macros:
					return module + " " + Pretty(((MacroParameters)parameter).ToString());
				case Module.ModMatrix:
					return module + " " + Pretty(((ModMatrixParameters)parameter).ToString());
				default:
					return "";
			}
		}
	}
}
