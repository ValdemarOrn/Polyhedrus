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

		float Am12;
		float Am23;
		float Fm12;
		float Fm13;
		float Fm23;
		float Noise;
		float Output;

		float Am12Mod;
		float Am23Mod;
		float Fm12Mod;
		float Fm13Mod;
		float Fm23Mod;
		float NoiseMod;
		float OutputMod;

		int Color;
	};
}

#endif

