#ifndef LEIFTUR_MIXER_SETTINGS
#define LEIFTUR_MIXER_SETTINGS

namespace Leiftur
{
	class MixerSettings
	{
	public:
		float Osc1Volume;
		float Osc2Volume;
		float Osc3Volume;

		float Osc1VolumeMod;
		float Osc2VolumeMod;
		float Osc3VolumeMod;

		float Osc1Pan;
		float Osc2Pan;
		float Osc3Pan;

		float Osc1PanMod;
		float Osc2PanMod;
		float Osc3PanMod;

		float MixerAm12;
		float MixerAm23;
		float MixerFm12;
		float MixerFm13;
		float MixerFm23;
		float MixerNoise;
		float MixerOutput;

		float MixerAm12Mod;
		float MixerAm23Mod;
		float MixerFm12Mod;
		float MixerFm13Mod;
		float MixerFm23Mod;
		float MixerNoiseMod;
		float MixerOutputMod;
	};
}

#endif

