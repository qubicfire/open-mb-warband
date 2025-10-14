#include "utils/assert.h"
#include "utils/profiler.h"
#include "engine.h"

#include "core/managers/assets.h"
#include "core/graphics/renderer.h"
#include "core/graphics/text_builder_3d.h"

#include "core/managers/objects.h"

#include "core/managers/input.h"
#include "core/managers/time.h"

#ifdef _DEBUG
	#include "core/graphics/imgui/imgui.h"
#endif // _DEBUG

#include "core/net/server.h"
#include "core/net/client.h"

#include "game/objects/test.h"

#include "core/managers/physics.h"

#include "utils/mb_string.h"

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
	using namespace mbcore;

	g_engine = this;

#ifdef _DEBUG
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
#endif // _DEBUG

	const WindowProperties properties = { this, m_title, m_width, m_height };
	m_window = Window::create(properties);

	Input::initialize(m_window);
	
	g_platform = Platform::create();

	// TODO: Connecting to singleplayer for test. Remove later
	Server::connect(ServerType::Single);

	g_physics->load();

	if (Client::is_running())
	{
		construct_global_unique(TextBuilder3D, text_builder_3d);

		g_text_builder_3d->load();
	}
}

void Engine::on_resized(const uint32_t width, const uint32_t height)
{
	m_width = width;
	m_height = height;

	Renderer::update_view_matrix();
}

#ifdef _DEBUG
	#include <GL/glew.h>
#endif

#include "game/scenes/map_scene.h"

void Engine::run()
{
	//g_scripts->compile();
	//g_scripts->call("game_get_statistics_line");

	g_assets->load_resource("test/map_icons_b.brf");
	g_assets->load_resource("test/map_icons_c.brf");
	g_assets->load_resource("test/map_flags.brf");
	g_assets->load_resource("test/map_flags_b.brf");
	g_assets->load_resource("test/map_flags_c.brf");
	g_assets->load_resource("test/map_flags_d.brf");
	g_assets->load_resource("test/map_icon_meshes.brf");
	g_assets->load_resource("test/map_tree_meshes.brf");

	g_assets->load_shader("main",
		"test/vs_main.glsl", 
		"test/ps_main.glsl");
	g_assets->load_shader("map_terrain",
		"test/vs_map_terrain.glsl",
		"test/ps_map_terrain.glsl");
	g_assets->load_shader("text_3d",
		"test/vs_text_3d.glsl",
		"test/ps_text_3d.glsl");
#ifdef _DEBUG
	g_assets->load_shader("map_terrain_debug",
		"test/vs_map_terrain.glsl",
		"test/ps_map_debug.glsl");
	g_assets->load_shader("test",
		"test/vs_test.glsl",
		"test/ps_test.glsl");
#endif // _DEBUG

	g_assets->wait_async_signal();

	m_tree.push<MapScene>();

	while (m_is_running)
	{
		Renderer::reset();
		Time::process_next_frame();
		m_window->clear();

		profiler_start(profiler_client);
		if (Client::is_running())
			g_client->update(&m_tree);
		profiler_stop(profiler_client, false);

#ifdef _DEBUG
		ImGui::NewFrame();
		ImGui::Begin("Props");
		{
			if (Test* prop = g_objects->find<Test>())
			{
				glm::vec3 origin = prop->get_origin();
				const glm::vec3& rotation = prop->get_rotation();
				ImGui::Text("Origin: %.3f %.3f %.3f", origin.x, origin.y, origin.z);
				ImGui::Text("Rotation: %.3f %.3f %.3f", rotation.x, rotation.y, rotation.z);

				if (ImGui::InputFloat3("Origin", glm::value_ptr(origin)))
					prop->set_origin(origin);
			}
		}
		ImGui::End();
#endif // _DEBUG

		profiler_start(profiler_server);
		if (Server::is_running())
			g_server->update(&m_tree);
		profiler_stop(profiler_server, false);

		profiler_start(profiler_physics);
		g_physics->update();
		profiler_stop(profiler_physics, false);

#ifdef _DEBUG
		static bool s_disable_draw = false;
		static bool s_wireframe = false;
		static bool s_cull_back = false;
		static bool s_vsync = true;

		if (s_wireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		if (s_cull_back)
			glCullFace(GL_FRONT);

		m_window->set_vsync(s_vsync);

		profiler_start(profiler_draw);
		if (Client::is_running())
		{
			if (!s_disable_draw)
				g_objects->draw_all();
		}
		profiler_stop(profiler_draw, false);
#else 
		g_objects->draw_all();
#endif

#ifdef _DEBUG
		if (s_wireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		if (s_cull_back)
			glCullFace(GL_BACK);

		ImGui::Begin("Server/Client");
		{
			if (ImGui::Button("Host server"))
				Server::connect(ServerType::PTP, "localhost", 3000);

			if (ImGui::Button("Connect to server"))
				Client::connect(ClientType::PTP, "localhost", 3000);

			if (ImGui::Button("Singleplayer mode"))
				Server::connect(ServerType::Single);

			if (ImGui::Button("Disconnect"))
			{
				Client::disconnect();

				Server::connect(ServerType::Single);
			}
		}
		ImGui::End();

		ImGui::Begin("Base Engine Debug Info");
		{
			ImGui::Text("Frames: %d", Time::get_frame());
			ImGui::Text("Delta time: %.016f", Time::get_delta_time());
			ImGui::Text("Time: %.f", Time::get_time());
			ImGui::Text("Fps: %.f", Time::get_fps());
			ImGui::Text("Draw calls: %d", Renderer::get_draw_calls());
			ImGui::Text("Client update: %f", profiler_client.get_time());
			ImGui::Text("Server update: %f", profiler_server.get_time());
			ImGui::Text("Physics update: %f", profiler_physics.get_time());
			ImGui::Text("Renderer update: %f", profiler_draw.get_time());

			ImGui::Checkbox("Disable mesh renderer", &s_disable_draw);
			ImGui::Checkbox("Disable vsync", &s_vsync);
			ImGui::Checkbox("Enable renderer aabb", &ObjectManager::m_is_aabb_enabled);
			ImGui::Checkbox("Enable wireframe", &s_wireframe);
			ImGui::Checkbox("Enable back cull face", &s_cull_back);
			ImGui::Checkbox("Enable Physics Debug", &Physics::m_is_debug_draw);
		}
		ImGui::End();

		ImGui::Render();
#endif

		m_window->update();
	}

	Server::disconnect();
	Client::disconnect();
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
