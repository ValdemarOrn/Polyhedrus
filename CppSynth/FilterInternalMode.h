#ifndef LEIFTUR_FILTER_INTERNAL_MODE
#define LEIFTUR_FILTER_INTERNAL_MODE

namespace Leiftur
{
	enum class InternalFilterMode
	{
		Lp24 = 0, // default
		Lp18,
		Lp12,
		Lp6,

		Hp24,
		Hp18,
		Hp12,
		Hp6,

		Bp6_6,
		Bp6_12,
		Bp12_6,

		Bp12_12,
		Bp6_18,
		Bp18_6,

		Count
	};
}

#endif