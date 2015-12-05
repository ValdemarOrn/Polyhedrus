#ifndef POLYHEDRUS_MOD_SOURCE_DEST
#define POLYHEDRUS_MOD_SOURCE_DEST

enum class ModSource
{
	Off = 0,
	EnvAmp = 1,
	EnvFilter = 2,

	Mod1 = 10,
	Mod2 = 11,
	Mod3 = 12,
	Mod1Unipolar = 13,
	Mod2Unipolar = 14,
	Mod3Unipolar = 15,
	Mod1Env = 16,
	Mod2Env = 17,
	Mod3Env = 18,

	SlopGen1 = 20,
	SlopGen2 = 21,
	SlopGen3 = 22,

	KeyTrack = 30,
	KeyTrackUnipolar = 31,
	Pitchbend = 32,

	Velocity = 40,
	Gate = 41,
		
	ModWheel = 50,
	KeyPressure = 51,
	ChannelPressure = 52,

	VoiceIndex = 60,
	UnisonIndex = 61,

	Count = 62
};

enum class ModDest
{
	Off = 0,

	OscAllPitch = 10,
	OscAllShape = 11,
	OscAllPan = 12,
	OscAllVolume = 13,

	Osc1Pitch = 20,
	Osc1Shape = 21,
	Osc1Pan = 22,
	Osc1Volume = 23,

	Osc2Pitch = 30,
	Osc2Shape = 31,
	Osc2Pan = 32,
	Osc2Volume = 33,

	Osc3Pitch = 40,
	Osc3Shape = 41,
	Osc3Pan = 42,
	Osc3Volume = 43,

	MixerAm12 = 60,
	MixerAm23 = 61,
	MixerFm12 = 62,
	MixerFm13 = 63,
	MixerFm23 = 64,
	MixerNoise = 65,
	MixerCharacterOut = 66,
	MixerFilterHpOut = 67,
	MixerFilterMainOut = 68,

	//CharacterBottom = 70,
	//CharacterTop = 71,
	//CharacterDecimate = 72,
	//CharacterReduce = 73,
	//CharacterClip = 74,

	FilterHpCutoff = 80,

	FilterMainCutoff = 90,
	FilterMainDrive = 91,
	FilterMainResonance = 92,

	EnvAmpAttack = 100,
	EnvAmpHold = 101,
	EnvAmpDecay = 102,
	EnvAmpSustain = 103,
	EnvAmpRelease = 104,

	EnvFilterAttack = 110,
	EnvFilterHold = 111,
	EnvFilterDecay = 112,
	EnvFilterSustain = 113,
	EnvFilterRelease = 114,

	/*Lfo1Phase = 120,
	Lfo1Freq = 121,
	Lfo1Attack = 122,
	Lfo1Decay = 123,
	Lfo1Sustain = 124,

	Lfo2Phase = 130,
	Lfo2Freq = 131,
	Lfo2Attack = 132,
	Lfo2Decay = 133,
	Lfo2Sustain = 134,

	ArpGate = 140,
	ArpTempo = 141,*/

	Count = 150

};

#endif
