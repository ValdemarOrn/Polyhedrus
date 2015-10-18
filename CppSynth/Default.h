#ifndef LEIFTUR_DEFAULT
#define LEIFTUR_DEFAULT

#include <stdint.h>

// --------- compiler target ---------

#ifdef _MSC_VER

	// ---------- Define #pragma WARNING -----------
	#define STRINGIZE_HELPER(x) #x
	#define STRINGIZE(x) STRINGIZE_HELPER(x)
	#define WARNING(desc) message(__FILE__ "(" STRINGIZE(__LINE__) ") : Warning: " #desc)

	#include "inttypes.h"

	#define __dllexport __declspec(dllexport)

	#ifdef DEBUG
		#define __inline_always inline
	#else
		#define __inline_always __forceinline
	#endif

#endif


#ifdef __GNUC__

	#include<stdint.h>

	#define __dllexport 
	#define __stdcall __attribute__((stdcall))

	#ifdef DEBUG
		#define __inline_always inline
	#else
		#define __inline_always inline __attribute__((always_inline))
	#endif

#endif

#endif
