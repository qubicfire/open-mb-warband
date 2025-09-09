#ifndef _MB_CONFIG_H
#define _MB_CONFIG_H

#include <iomanip>

#ifndef MB_FORCE_INLINE
	#ifdef _MSC_VER
		#define MB_INLINE __forceinline
	#endif
#endif // !MB_FORCE_INLINE


#ifndef MB_NOEXCEPT
	#ifdef _MSC_VER
		#define MB_NOEXCEPT noexcept
	#endif
#endif // !MB_FORCE_INLINE

#ifndef MB_NODISCARD
	#define MB_NODISCARD [[nodiscard]]
#endif // !MB_DISCARD

#endif // !_MB_CONFIG_H
