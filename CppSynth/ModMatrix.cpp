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
	}

	void ModMatrix::CreateFixedRoutes()
	{
		FixedRoutes[FixedRouteOsc1Pitchbend].Source = ModSource::Pitchbend;
		FixedRoutes[FixedRouteOsc1Pitchbend].Amount = 2 / 24.0;
		FixedRoutes[FixedRouteOsc1Pitchbend].Destination = ModDest::Osc1Pitch;
		
		FixedRoutes[FixedRouteOsc2Pitchbend].Source = ModSource::Pitchbend;
		FixedRoutes[FixedRouteOsc2Pitchbend].Amount = 2 / 24.0;
		FixedRoutes[FixedRouteOsc2Pitchbend].Destination = ModDest::Osc2Pitch;

		FixedRoutes[FixedRouteOsc3Pitchbend].Source = ModSource::Pitchbend;
		FixedRoutes[FixedRouteOsc3Pitchbend].Amount = 2 / 24.0;
		FixedRoutes[FixedRouteOsc3Pitchbend].Destination = ModDest::Osc3Pitch;

		FixedRoutes[FixedRouteFilterHpKeytrack].Source = ModSource::KeyTrack;
		FixedRoutes[FixedRouteFilterHpKeytrack].Amount = 0.1; // Todo: Tune filters
		FixedRoutes[FixedRouteFilterHpKeytrack].Destination = ModDest::FilterHpCutoff;

		FixedRoutes[FixedRouteFilterMainKeytrack].Source = ModSource::KeyTrack;
		FixedRoutes[FixedRouteFilterMainKeytrack].Amount = 0.1; // Todo: Tune filters
		FixedRoutes[FixedRouteFilterMainKeytrack].Destination = ModDest::FilterMainCutoff;

		FixedRoutes[FixedRouteFilterHpEnv].Source = ModSource::EnvFilter;
		FixedRoutes[FixedRouteFilterHpEnv].Amount = 0.0;
		FixedRoutes[FixedRouteFilterHpEnv].Destination = ModDest::FilterHpCutoff;

		FixedRoutes[FixedRouteFilterMainEnv].Source = ModSource::EnvFilter;
		FixedRoutes[FixedRouteFilterMainEnv].Amount = 0.0;
		FixedRoutes[FixedRouteFilterMainEnv].Destination = ModDest::FilterMainCutoff;
	}

	__inline_always void ModMatrix::ApplyRoute(ModRoute * route)
	{
		auto source = ModSourceValues[(int)route->Source];
		auto viaSource = ModSourceValues[(int)route->ViaSource];
		auto modValue = source * ((1 - route->ViaAmount) + viaSource * route->ViaAmount);
		ModDestinationValues[(int)route->Destination] += modValue * route->Amount;
	}
}
