#ifdef _DEBUG
	#include "core/graphics/imgui/imgui_impl_glfw.h"
	#include "core/graphics/imgui/imgui_impl_opengl3.h"
#endif
#include "core/engine/engine.h"
#include "core/managers/input.h"

#include "utils/assert.h"
#include "utils/log.h"

#include "opengl_window.h"

using namespace mbcore;

OpenGLWindow::OpenGLWindow(const WindowProperties& properties)
{
	initialize(properties);
}

OpenGLWindow::~OpenGLWindow()
{
	glfwDestroyWindow(m_window);
	glfwTerminate();
}

void OpenGLWindow::initialize(const WindowProperties& properties)
{
	core_assert(glfwInit(), "%s", "Failed to start a glfw");
	log_success("GLFW initialized");

#ifdef _DEBUG
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#else
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif // _DEBUG

	m_window = glfwCreateWindow(properties.m_width, 
		properties.m_height,
		properties.m_title.data(),
		nullptr,
		nullptr);

	core_assert(m_window != nullptr, "%s", "Failed to create a window");
	log_success("Game window is created");

	glfwMakeContextCurrent(m_window);

	core_assert(glewInit() == GLEW_OK, "%s", "Failed to start a glew");
	log_success("Glew initialized");

	glewExperimental = true;

	glfwSetWindowUserPointer(m_window, properties.m_engine);

	glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, int width, int height) {
		Engine* engine = static_cast<Engine*>(
			glfwGetWindowUserPointer(window)
		);

		glViewport(0, 0, width, height);
		engine->on_resized(width, height);
	});

	glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int code, int action, int mods) {
		switch (action)
		{
			case GLFW_PRESS:
				InputInternal::set_key_pressed(key);
				break;
			case GLFW_RELEASE:
				InputInternal::set_key_released(key);
				break;
		}
	});

	glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int mods) {
		switch (action)
		{
			case GLFW_PRESS:
				InputInternal::set_mouse_button_pressed(button);
				break;
			case GLFW_RELEASE:
				InputInternal::set_mouse_button_released(button);
				break;
		}
	});

	glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double x, double y) {
		Input::set_mouse_origin(x, y);
	});

	glfwSetWindowCloseCallback(m_window, [](GLFWwindow* window) {
		Engine* engine = static_cast<Engine*>(
			glfwGetWindowUserPointer(window)
		);

		engine->quit();
	});

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glDepthFunc(GL_LEQUAL);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

#ifdef _DEBUG
	ImGui_ImplGlfw_InitForOpenGL(m_window, true);
	ImGui_ImplOpenGL3_Init();
#endif
}

void OpenGLWindow::clear()
{
	glfwPollEvents();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

#ifdef _DEBUG
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
#endif // _DEBUG
}

void OpenGLWindow::update()
{
#ifdef _DEBUG
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif

	glfwSwapBuffers(m_window);
}

void OpenGLWindow::set_cursor_visible(bool state)
{
	glfwSetInputMode(m_window, GLFW_CURSOR, state ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
}

void OpenGLWindow::set_vsync(bool state)
{
	glfwSwapInterval(state);
}

#ifdef _DEBUG
void OpenGLWindow::set_wireframe(bool state)
{
	glPolygonMode(GL_FRONT_AND_BACK, state ? GL_LINE : GL_FILL);
}

void OpenGLWindow::set_cull(mbcore::CullFace cull)
{
	glCullFace(cull == mbcore::CullFace::Back ? GL_BACK : GL_FRONT);
}
#endif

float OpenGLPlatform::get_time() const
{
	return static_cast<float>(
		glfwGetTime()
	);
}
