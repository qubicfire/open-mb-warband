#ifndef _WINDOW_H
#define _WINDOW_H
#include <string_view>

#include "core/mb_type_traits.h"

class Engine;

namespace mbcore
{
	struct WindowProperties
	{
		Engine* m_engine;
		std::string_view m_title;
		uint32_t m_width;
		uint32_t m_height;
	};

	struct Window
	{
		virtual void initialize(const WindowProperties& properties) = 0;
		virtual void clear() = 0;
		virtual void update() = 0;

		virtual void set_cursor_visible(bool state) = 0;
		virtual void set_vsync(bool state) = 0;

		static mb_unique<Window> create(const WindowProperties& properties);
	};

	struct Platform
	{
		virtual float get_time() const = 0;

		static mb_unique<Platform> create();
	};

	declare_global_unique_class(Platform, platform)
}

#endif // !_WINDOW_H
