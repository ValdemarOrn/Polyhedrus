#include "ModMatrix.h"

namespace Leiftur
{
	ModMatrix::ModMatrix()
	{
		ModSourceValues[(int)ModSource::Off] = 1.0;
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
			auto route = &FixedRoutes[i];
			auto source = ModSourceValues[(int)route->Source];
			auto viaSource = ModSourceValues[(int)route->ViaSource];
			auto modValue = source * ((1 - route->ViaAmount) + viaSource * route->ViaAmount);
			ModDestinationValues[(int)route->Destination] += modValue * route->Amount;
		}

		for (size_t i = 0; i < MatrixCount; i++)
		{
			auto route = &Matrix[i];
			auto source = ModSourceValues[(int)route->Source];
			auto viaSource = ModSourceValues[(int)route->ViaSource];
			auto modValue = source * ((1 - route->ViaAmount) + viaSource * route->ViaAmount);
			ModDestinationValues[(int)route->Destination] += modValue * route->Amount;
		}
	}
}
