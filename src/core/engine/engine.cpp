#include "utils/assert.h"
#include "engine.h"

#include "core/managers/assets.h"
#include "core/graphics/renderer.h"

#include "core/managers/objects.h"
#include "core/objects/camera.h"
#include "core/objects/prop.h"
#include "core/objects/map.h"

#include "core/managers/input.h"
#include "core/managers/time.h"

#ifdef _DEBUG
	#include "core/graphics/imgui/imgui.h"
#endif // _DEBUG

#include "core/net/server_interface.h"
#include "core/net/client_interface.h"

#include "platform.h"

Engine::Engine(std::string_view title, 
	const uint32_t width,
	const uint32_t height)
	: m_title(title)
	, m_width(width)
	, m_height(height)
	, m_is_running(true)
{
	initialize();
}

void Engine::initialize()
{
#ifdef _DEBUG
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
#endif // _DEBUG

	const mbcore::WindowProperties properties = { this, m_title, m_width, m_height };
	m_window = mbcore::Window::create(properties);

	Input::initialize(m_window);
	Platform::initialize();

	g_engine = this;
}

void Engine::on_resized(const uint32_t width, const uint32_t height)
{
	m_width = width;
	m_height = height;

	Renderer::update_view_matrix();
}

void Engine::on_key_pressed(const int key)
{
	Input::set_key_pressed(key);
}

void Engine::on_key_released(const int key)
{
	Input::set_key_released(key);
}

void Engine::on_mouse_pressed(const int button)
{
	Input::set_mouse_button_pressed(button);
}

void Engine::on_mouse_released(const int button)
{
	Input::set_mouse_button_released(button);
}

void Engine::on_mouse_origin_changed(const double x, const double y)
{
	Input::set_mouse_origin(x, y);
}

bool Engine::load_module(const std::string& path)
{

	return true;
}

#ifdef _DEBUG
#include <GL/glew.h>

static void imgui_model_info(Prop* prop)
{
	if (ImGui::Begin("Model Info") && prop != nullptr)
	{
		const brf::Mesh* mesh = prop->get_mesh();
		ImGui::Text(mesh->get_name().c_str());

		const auto& [format, memory] = mesh->get_memory_size();
		ImGui::Text(format, memory);
		ImGui::Text("Indices: %d", prop->get_vertex_array()->get_index_buffer()->m_count);
		ImGui::Text("Id: %d", prop->get_id());
	}
	ImGui::End();
}
#endif // _DEBUG

#include "game/scenes/map_scene.h"

void Engine::run()
{
	//g_scripts->compile();
	//g_scripts->call("game_get_statistics_line");

	g_assets->load_resource_async("test/map_icons_b.brf");
	g_assets->load_resource_async("test/map_icons_c.brf");
	g_assets->load_resource_async("test/map_flags.brf");
	g_assets->load_resource_async("test/map_flags_b.brf");
	g_assets->load_resource_async("test/map_flags_c.brf");
	g_assets->load_resource_async("test/map_flags_d.brf");
	g_assets->load_resource_async("test/map_icon_meshes.brf");
	g_assets->load_resource_async("test/map_tree_meshes.brf");

	g_assets->load_shader("main",
		"test/vs_main.glsl", 
		"test/ps_main.glsl");
	g_assets->load_shader("map_terrain",
		"test/vs_map_terrain.glsl",
		"test/ps_map_terrain.glsl");
	g_assets->load_shader("test",
		"test/vs_test.glsl",
		"test/ps_test.glsl");

	g_assets->wait_async_signal();

	m_tree.push<MapScene>();

	while (m_is_running)
	{
		Renderer::reset();
		Time::process_next_frame();
		m_window->clear();

#ifdef _DEBUG
		ImGui::NewFrame();
#endif // _DEBUG

		if (g_client_interface)
			g_client_interface->update();

		m_tree.update();

		if (g_server_interface)
			g_server_interface->update();

#ifdef _DEBUG
		static bool s_wireframe = false;
		static bool s_cull_back = false;
		static Prop* s_prop = nullptr;

		if (s_wireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		if (s_cull_back)
			glCullFace(GL_FRONT);
#endif 
		// TODO: Implement scene system
		// client->update();
		// if host != null
		// host->update();
		g_objects->draw_all();

#ifdef _DEBUG
		if (s_wireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		if (s_cull_back)
			glCullFace(GL_BACK);

		imgui_model_info(s_prop);

		ImGui::Begin("Server/Client");
		{
			if (ImGui::Button("Host server"))
				ServerInterface::connect("localhost", 3000, ServerType::PTP);

			if (ImGui::Button("Connect to server"))
				ClientInterface::connect("localhost", 3000);

			char buffer[16] {};
			if (ImGui::InputText("Message", buffer, sizeof(buffer), ImGuiInputTextFlags_EnterReturnsTrue))
			{
				MessagePacket packet;
				packet.m_message = buffer;

				g_server_interface->broadcast(packet);
			}
		}
		ImGui::End();

		ImGui::Begin("Base Engine Debug Info");
		{
			ImGui::Text("Frames: %d", Time::get_frame());
			ImGui::Text("Delta time: %.016f", Time::get_delta_time());
			ImGui::Text("Time: %.f", Time::get_time());
			ImGui::Text("Draw calls: %d", Renderer::get_draw_calls());

			ImGui::Checkbox("Enable wireframe", &s_wireframe);
			ImGui::Checkbox("Enable back cull face", &s_cull_back);
		}
		ImGui::End();

		ImGui::Begin("Props");
		{
			for (const auto& prop : g_objects->find_all<Prop>())
			{
				if (!prop->get_mesh())
					continue;

				const std::string& name = prop->get_mesh()->get_name();
				if (ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_Bullet))
				{
					if (ImGui::IsItemClicked())
						s_prop = prop;

					ImGui::TreePop();
				}
			}
		}
		ImGui::End();

		ImGui::Render();
#endif

		m_window->update();
	}

	if (g_server_interface)
		g_server_interface->dispose();

	if (g_client_interface)
		g_client_interface->dispose();
}

void Engine::quit()
{
	m_is_running = false;
}

const std::string_view Engine::get_title() const
{
	return m_title;
}

const uint32_t Engine::get_width() const
{
	return m_width;
}

const uint32_t Engine::get_height() const
{
	return m_height;
}

const float Engine::get_aspect_ratio() const
{
	return static_cast<float>(m_width) / static_cast<float>(m_height);
}
