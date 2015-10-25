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
		Character = 15,

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
	}

	enum OscParameters
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

	enum EnvParameters
	{
		Attack = 1,
		Hold = 2,
		Decay = 3,
		Sustain = 4,
		Release = 5,
		Velocity = 6,
		Retrigger = 7,
		Exponent = 8,
	}

	enum LfoParameters
	{
		Phase = 1,
		Freq = 2,
		Shape = 3,
		Attack = 4,
		Decay = 5,
		Sustain = 6,
		Release = 7,
		Sync = 8,
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
				case Module.Character:
					parameter = (int)(CharacterParameters)Enum.Parse(typeof(CharacterParameters), parts[1], true);
					break;
				case Module.FilterHp:
					parameter = (int)(FilterHpParameters)Enum.Parse(typeof(FilterHpParameters), parts[1], true);
					break;
				case Module.FilterMain:
					parameter = (int)(FilterMainParameters)Enum.Parse(typeof(FilterMainParameters), parts[1], true);
					break;
				case Module.EnvAmp:
				case Module.EnvFilter:
				case Module.EnvMod:
					parameter = (int)(EnvParameters)Enum.Parse(typeof(EnvParameters), parts[1], true);
					break;
				case Module.Lfo1:
				case Module.Lfo2:
					parameter = (int)(LfoParameters)Enum.Parse(typeof(LfoParameters), parts[1], true);
					break;
				case Module.Arp:
					parameter = (int)(ArpParameters)Enum.Parse(typeof(ArpParameters), parts[1], true);
					break;
				case Module.Voices:
					parameter = (int)(VoiceParameters)Enum.Parse(typeof(VoiceParameters), parts[1], true);
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
				case Module.EnvAmp:
				case Module.EnvFilter:
				case Module.EnvMod:
					return module + " " + Pretty(((EnvParameters)parameter).ToString());
				case Module.Lfo1:
				case Module.Lfo2:
					return module + " " + Pretty(((LfoParameters)parameter).ToString());
				case Module.Arp:
					return module + " " + Pretty(((ArpParameters)parameter).ToString());
				case Module.Voices:
					return module + " " + Pretty(((VoiceParameters)parameter).ToString());
				case Module.ModMatrix:
					return module + " " + Pretty(((ModMatrixParameters)parameter).ToString());
				default:
					return "";
			}
		}
	}
}
