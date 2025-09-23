#ifndef _RENDERER_H
#define _RENDERER_H
#include "shader.h"
#include "core/platform/context.h"

#include "core/objects/camera.h"

class Renderer final
{
public:
	enum : uint8_t
	{
		OpenGL,
		DirectX9, // not implemented
		DirectX11, // not implemented
		Vulkan, // not implemented
	};
	static inline uint8_t API = OpenGL; // DO NOT TOUCH

	static void setup_camera_object(Camera* camera) noexcept;
	static void update_view_matrix();

	static void draw_vertex_array(const mb_unique<mbcore::VertexArray>& array);
	static void dispatch_compute(Shader* shader,
		const int groups_x,
		const int groups_y = 1,
		const int groups_z = 1);

	static void reset();

#ifdef _DEBUG
	static Camera* get_camera();
#endif // _DEBUG
	static uint32_t get_draw_calls();
private:
	static inline mb_unique<mbcore::RendererContext> m_context = mbcore::RendererContext::create();
	static inline Camera* m_camera = nullptr;
	static inline uint32_t m_draw_calls {};
};

#endif // !_RENDERER_H
