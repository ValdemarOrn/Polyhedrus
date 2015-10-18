#ifndef LEIFTUR_MOD_SOURCE_DEST
#define LEIFTUR_MOD_SOURCE_DEST

enum class ModSource
{
	Off = 0,
	EnvAmp = 1,
	EnvFilter = 2,
	EnvMod = 3,

	Lfo1 = 10,
	Lfo2 = 11,
	Lfo1Unipolar = 12,
	Lfo2Unipolar = 13,

	KeyTrack = 20,
	KeyTrackUnipolar = 21,
	Pitchbend = 22,

	Velocity = 30,
	Gate = 31,
		
	ModWheel = 40,
	KeyPressure = 41,
	ChannelPressure = 42,

	VoiceIndex = 50,
	UnisonIndex = 51,

	Count = 52
};

enum class ModDest
{
	Off = 0,

	Osc1Pitch = 10,
	Osc1Shape = 11,
	Osc1Pan = 12,
	Osc1Volume = 13,

	Osc2Pitch = 20,
	Osc2Shape = 21,
	Osc2Pan = 22,
	Osc2Volume = 23,

	Osc3Pitch = 30,
	Osc3Shape = 31,
	Osc3Pan = 32,
	Osc3Volume = 33,

	MixerAm12 = 40,
	MixerAm23 = 41,
	MixerFm12 = 42,
	MixerFm13 = 43,
	MixerFm23 = 44,
	MixerNoise = 45,
	MixerOutput = 46,

	//CharacterBottom = 50,
	//CharacterTop = 51,
	//CharacterDecimate = 52,
	//CharacterReduce = 53,
	//CharacterClip = 54,

	FilterHpCutoff = 60,

	FilterMainCutoff = 70,
	FilterMainDrive = 71,
	FilterMainResonance = 72,

	//EnvAmp = 80
	//EnvFilter = 90
	//EnvMod = 100

	/*Lfo1Phase = 110,
	Lfo1Freq = 111,
	Lfo1Attack = 112,
	Lfo1Decay = 113,
	Lfo1Sustain = 114,

	Lfo2Phase = 120,
	Lfo2Freq = 121,
	Lfo2Attack = 122,
	Lfo2Decay = 123,
	Lfo2Sustain = 124,

	ArpGate = 130,
	ArpTempo = 131,*/

	Count = 132

};

#endif
