#ifndef _OPENGL_WINDOW_H
#define _OPENGL_WINDOW_H
#include "core/platform/window.h"

#include "opengl.h"

class OpenGLWindow : public mbcore::Window
{
public:
	OpenGLWindow(const mbcore::WindowProperties& properties);
	~OpenGLWindow();

	void initialize(const mbcore::WindowProperties& properties) override;
	void clear() override;
	void update() override;

	void set_cursor_visible(bool state) override;
	void set_vsync(bool state) override;
private:
	GLFWwindow* m_window;
};

#endif // !_OPENGL_WINDOW_H
