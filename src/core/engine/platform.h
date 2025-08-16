#ifndef _PLATFORM_H
#define _PLATFORM_H
#include "core/platform/window.h"

class Platform
{
	friend class Engine;
public:
	static float get_time()
	{
		return mbcore::g_platform->get_time();
	}
private:
	static void initialize()
	{
		mbcore::g_platform = mbcore::Platform::create();
	}
};

#endif // !_PLATFORM_H
