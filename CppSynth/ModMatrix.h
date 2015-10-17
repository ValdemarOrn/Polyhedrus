#ifndef LEIFTUR_MOD_MATRIX
#define LEIFTUR_MOD_MATRIX

#include "ModSourceDest.h"

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
		static const int FixedRouteCount = 8;
		static const int MatrixCount = 16;

		float ModSourceValues[(unsigned int)ModSource::Count];
		float ModDestinationValues[(unsigned int)ModDest::Count];
	private:
		ModRoute FixedRoutes[FixedRouteCount];
		ModRoute Matrix[MatrixCount];
	public:
		ModMatrix();
		~ModMatrix();

		void Process();

	};
}
#endif

