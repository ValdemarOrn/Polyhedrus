#include "ModMatrix.h"

namespace Leiftur
{
	ModMatrix::ModMatrix()
	{
		ModSourceValues[(int)ModSource::Off] = 1.0;

		CreateFixedRoutes();
	}

	ModMatrix::~ModMatrix()
	{
	}

	void ModMatrix::Process()
	{
		for (size_t i = 0; i < (int)ModDest::Count; i++)
			ModDestinationValues[i] = 0.0;
		
		for (size_t i = 0; i < FixedRouteCount; i++)
		{
			ApplyRoute(&FixedRoutes[i]);
		}

		for (size_t i = 0; i < MatrixCount; i++)
		{
			ApplyRoute(&Matrix[i]);
		}

		SumOscAllRoutes();
	}

	void ModMatrix::CreateFixedRoutes()
	{
		FixedRoutes[FixedRouteOscAllPitchbend].Source = ModSource::Pitchbend;
		FixedRoutes[FixedRouteOscAllPitchbend].Amount = 2 / 24.0;
		FixedRoutes[FixedRouteOscAllPitchbend].Destination = ModDest::OscAllPitch;
		
		FixedRoutes[FixedRouteOscAllUnisonDetune].Source = ModSource::UnisonIndex;
		FixedRoutes[FixedRouteOscAllUnisonDetune].Amount = 0.2 / 12.0;
		FixedRoutes[FixedRouteOscAllUnisonDetune].Destination = ModDest::OscAllPitch;

		FixedRoutes[FixedRouteOscAllUnisonSpread].Source = ModSource::UnisonIndex;
		FixedRoutes[FixedRouteOscAllUnisonSpread].Amount = 0.5;
		FixedRoutes[FixedRouteOscAllUnisonSpread].Destination = ModDest::OscAllPan;

		FixedRoutes[FixedRouteFilterHpKeytrack].Source = ModSource::KeyTrack;
		FixedRoutes[FixedRouteFilterHpKeytrack].Amount = 1.0;
		FixedRoutes[FixedRouteFilterHpKeytrack].Destination = ModDest::FilterHpCutoff;

		FixedRoutes[FixedRouteFilterMainKeytrack].Source = ModSource::KeyTrack;
		FixedRoutes[FixedRouteFilterMainKeytrack].Amount = 1.0;
		FixedRoutes[FixedRouteFilterMainKeytrack].Destination = ModDest::FilterMainCutoff;

		FixedRoutes[FixedRouteFilterHpEnv].Source = ModSource::EnvFilter;
		FixedRoutes[FixedRouteFilterHpEnv].Amount = 0.0;
		FixedRoutes[FixedRouteFilterHpEnv].Destination = ModDest::FilterHpCutoff;

		FixedRoutes[FixedRouteFilterMainEnv].Source = ModSource::EnvFilter;
		FixedRoutes[FixedRouteFilterMainEnv].Amount = 0.0;
		FixedRoutes[FixedRouteFilterMainEnv].Destination = ModDest::FilterMainCutoff;

		FixedRoutes[FixedRouteSlop1].Source = ModSource::SlopGen1;
		FixedRoutes[FixedRouteSlop1].Amount = 0.0;
		FixedRoutes[FixedRouteSlop1].Destination = ModDest::Osc1Pitch;

		FixedRoutes[FixedRouteSlop2].Source = ModSource::SlopGen2;
		FixedRoutes[FixedRouteSlop2].Amount = 0.0;
		FixedRoutes[FixedRouteSlop2].Destination = ModDest::Osc2Pitch;

		FixedRoutes[FixedRouteSlop3].Source = ModSource::SlopGen3;
		FixedRoutes[FixedRouteSlop3].Amount = 0.0;
		FixedRoutes[FixedRouteSlop3].Destination = ModDest::Osc3Pitch;
	}

	__inline_always void ModMatrix::ApplyRoute(ModRoute * route)
	{
		auto source = ModSourceValues[(int)route->Source];
		auto viaSource = ModSourceValues[(int)route->ViaSource];
		auto modValue = source * ((1 - route->ViaAmount) + viaSource * route->ViaAmount);
		ModDestinationValues[(int)route->Destination] += modValue * route->Amount;
	}

	__inline_always void ModMatrix::SumOscAllRoutes()
	{
		ModDestinationValues[(int)ModDest::Osc1Pan] += ModDestinationValues[(int)ModDest::OscAllPan];
		ModDestinationValues[(int)ModDest::Osc2Pan] += ModDestinationValues[(int)ModDest::OscAllPan];
		ModDestinationValues[(int)ModDest::Osc3Pan] += ModDestinationValues[(int)ModDest::OscAllPan];

		ModDestinationValues[(int)ModDest::Osc1Pitch] += ModDestinationValues[(int)ModDest::OscAllPitch];
		ModDestinationValues[(int)ModDest::Osc2Pitch] += ModDestinationValues[(int)ModDest::OscAllPitch];
		ModDestinationValues[(int)ModDest::Osc3Pitch] += ModDestinationValues[(int)ModDest::OscAllPitch];

		ModDestinationValues[(int)ModDest::Osc1Shape] += ModDestinationValues[(int)ModDest::OscAllShape];
		ModDestinationValues[(int)ModDest::Osc2Shape] += ModDestinationValues[(int)ModDest::OscAllShape];
		ModDestinationValues[(int)ModDest::Osc3Shape] += ModDestinationValues[(int)ModDest::OscAllShape];

		ModDestinationValues[(int)ModDest::Osc1Volume] += ModDestinationValues[(int)ModDest::OscAllVolume];
		ModDestinationValues[(int)ModDest::Osc2Volume] += ModDestinationValues[(int)ModDest::OscAllVolume];
		ModDestinationValues[(int)ModDest::Osc3Volume] += ModDestinationValues[(int)ModDest::OscAllVolume];
	}
}
