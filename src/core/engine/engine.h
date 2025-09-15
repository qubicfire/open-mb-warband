#ifndef _ENGINE_H
#define _ENGINE_H
#include "core/platform/window.h"

#include "core/mb.h"
#include "core/mb_type_traits.h"

#include "scene_tree.h"

class Engine final
{
public:
	explicit Engine(std::string_view title,
		const uint32_t width,
		const uint32_t height);

	void run();

	// Window events
	void on_resized(const uint32_t width, const uint32_t height);
	void on_key_pressed(const int key);
	void on_key_released(const int key);
	void on_mouse_pressed(const int button);
	void on_mouse_released(const int button);
	void on_mouse_origin_changed(const double x, const double y);

	void quit();

	const std::string_view get_title() const;
	const uint32_t get_width() const;
	const uint32_t get_height() const;
	const float get_aspect_ratio() const;
private:
	void initialize();

	bool load_module(const std::string& path);
private:
	std::string_view m_title;
	uint32_t m_width, m_height;
	bool m_is_running;

	mb_unique<mbcore::Window> m_window;
	SceneTree m_tree;
};

declare_global_class(Engine, engine)

#endif // !_ENGINE_H
