#ifndef _RENDERER_H
#define _RENDERER_H
#include <glm/ext.hpp>

#include "shader.h"
#include "core/platform/context.h"

#include "core/objects/camera.h"
#include "core/objects/model.h"

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
	static void build_model_projection(Shader* shader, 
		const glm::vec3& origin, 
		const glm::vec3& rotation,
		const glm::vec3& scale,
		const float angle);
	static void build_model_projection_only(Shader* shader,
		const glm::vec3& origin,
		const glm::vec3& rotation,
		const glm::vec3& scale,
		const float angle);
	static void update_view_matrix();

	static void draw_indexed(const Unique<mbcore::VertexArray>& array);
	static void draw_triangles(const Unique<mbcore::VertexArray>& array);
	static void reset();

#ifdef _DEBUG
	static Camera* get_camera();
#endif // _DEBUG
	static uint32_t get_draw_calls();
private:
	static inline Unique<mbcore::RendererContext> m_context = mbcore::RendererContext::create();
	static inline Camera* m_camera = nullptr;
	static inline uint32_t m_draw_calls {};
};

#endif // !_RENDERER_H
