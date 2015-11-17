#ifndef LEIFTUR_MIXER_SETTINGS
#define LEIFTUR_MIXER_SETTINGS

#include "AudioLib/Utils.h"

namespace Leiftur
{
	enum class RoutingStage
	{
		Character = 0,
		HpFilter = 1,
		MainFilter = 2,
		Direct = 3,
	};
	
	class MixerSettings
	{
	public:
		RoutingStage NoiseRouting;
		RoutingStage Osc1Routing;
		RoutingStage Osc2Routing;
		RoutingStage Osc3Routing;

		// ------------ Input Fields ---------------

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
		float CharacterOut;
		float FilterHpOut;
		float FilterMainOut;

		float Am12Mod;
		float Am23Mod;
		float Fm12Mod;
		float Fm13Mod;
		float Fm23Mod;
		float NoiseMod;
		float CharacterOutMod;
		float FilterHpOutMod;
		float FilterMainOutMod;

		// --------- Computed Fields ------------

		float Osc1VolL;
		float Osc1VolR;
		float Osc2VolL;
		float Osc2VolR;
		float Osc3VolL;
		float Osc3VolR;

		float Am12Total;
		float Am23Total;
		float Fm12Total;
		float Fm13Total;
		float Fm23Total;
		float NoiseTotal;
		float CharacterOutTotal;
		float FilterHpOutTotal;
		float FilterMainOutTotal;

		inline void ComputeOscVols()
		{
			float osc1Vol = AudioLib::Utils::LimitMin(Osc1Volume + Osc1VolumeMod, 0.0);
			float osc2Vol = AudioLib::Utils::LimitMin(Osc2Volume + Osc2VolumeMod, 0.0);
			float osc3Vol = AudioLib::Utils::LimitMin(Osc3Volume + Osc3VolumeMod, 0.0);
			float osc1Pan = Osc1Pan + Osc1PanMod;
			float osc2Pan = Osc2Pan + Osc2PanMod;
			float osc3Pan = Osc3Pan + Osc3PanMod;

			Osc1VolL = osc1Vol * AudioLib::Utils::Limit(-osc1Pan + 1, 0.0, 1.0);
			Osc1VolR = osc1Vol * AudioLib::Utils::Limit(osc1Pan + 1, 0.0, 1.0);

			Osc2VolL = osc2Vol * AudioLib::Utils::Limit(-osc2Pan + 1, 0.0, 1.0);
			Osc2VolR = osc2Vol * AudioLib::Utils::Limit(osc2Pan + 1, 0.0, 1.0);

			Osc3VolL = osc3Vol * AudioLib::Utils::Limit(-osc3Pan + 1, 0.0, 1.0);
			Osc3VolR = osc3Vol * AudioLib::Utils::Limit(osc3Pan + 1, 0.0, 1.0);

			Am12Total = AudioLib::Utils::Limit(Am12 + Am12Mod, 0.0, 1.0);
			Am23Total = AudioLib::Utils::Limit(Am23 + Am23Mod, 0.0, 1.0);
			Fm12Total = AudioLib::Utils::Limit(Fm12 + Fm12Mod, 0.0, 1.0);
			Fm13Total = AudioLib::Utils::Limit(Fm13 + Fm13Mod, 0.0, 1.0);
			Fm23Total = AudioLib::Utils::Limit(Fm23 + Fm23Mod, 0.0, 1.0);
			NoiseTotal = AudioLib::Utils::Limit(Noise + NoiseMod, 0.0, 1.0);
			CharacterOutTotal = AudioLib::Utils::Limit(CharacterOut + CharacterOutMod, 0.0, 1.0);
			FilterHpOutTotal = AudioLib::Utils::Limit(FilterHpOut + FilterHpOutMod, 0.0, 1.0);
			FilterMainOutTotal = AudioLib::Utils::Limit(FilterMainOut + FilterMainOutMod, 0.0, 1.0);
		}
	};
}

#endif

