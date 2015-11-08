#ifndef LEIFTUR_MOD_MATRIX
#define LEIFTUR_MOD_MATRIX

#include "ModSourceDest.h"
#include "Default.h"

namespace Leiftur
{
	class ModRoute
	{
	public:
		ModSource Source;
		ModDest Destination;
		ModSource ViaSource;
		float Amount;
		float ViaAmount;
	};

	class ModMatrix
	{
	public:
		static const int FixedRouteCount = 7;
		static const int MatrixCount = 16;

		static const int FixedRouteOscAllPitchbend     = 0;
		static const int FixedRouteOscAllUnisonDetune  = 1;
		static const int FixedRouteOscAllUnisonSpread  = 2;
		static const int FixedRouteFilterHpKeytrack    = 3;
		static const int FixedRouteFilterMainKeytrack  = 4;
		static const int FixedRouteFilterHpEnv         = 5;
		static const int FixedRouteFilterMainEnv       = 6;

		float ModSourceValues[(unsigned int)ModSource::Count];
		float ModDestinationValues[(unsigned int)ModDest::Count];
		ModRoute FixedRoutes[FixedRouteCount];
		ModRoute Matrix[MatrixCount];
	
	public:
		ModMatrix();
		~ModMatrix();
		void Process();

	private:
		void CreateFixedRoutes();
		void ApplyRoute(ModRoute* route);
		void SumOscAllRoutes();
	};
}
#endif

