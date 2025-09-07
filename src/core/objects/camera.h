#ifndef _CAMERA_H
#define _CAMERA_H
#include <glm/ext/matrix_float4x4.hpp>

#include "object.h"

#include "core/managers/physics.h"

struct Plane
{
	Plane() = default;

	Plane(const glm::vec3& point, const glm::vec3& normal)
		: m_normal(glm::normalize(normal))
		, m_distance(glm::dot(m_normal, point))
	{ }

	glm::vec3 m_normal{ 0.0f, 1.0f, 0.0f };
	float m_distance = 0.0f;
};

struct Frustum
{
	enum
	{
		PLANE_BACK = 0,
		PLANE_FRONT,
		PLANE_BOTTOM,
		PLANE_TOP,
		PLANE_RIGHT,
		PLANE_LEFT,
		PLANE_COUNT
	};

	Plane m_planes[PLANE_COUNT];
};

class Camera : public Object
{
	object_base(Camera)
public:
	void start_client() override;
	void update() override;
	void update_view_matrix();

	Frustum create_frustum() const;
	Ray screen_point_to_ray();

	void set_fov(const float fov);

	const float get_fov() const;
	const glm::vec3& get_front() const;
	const glm::vec3& get_right() const;
	const glm::vec3& get_up() const;
	const glm::mat4& get_projection() const;
	const glm::mat4& get_view() const;
private:
	float m_near;
	float m_far;
	float m_fov;
	float m_sensitivity;
	float m_last_offset_x;
	float m_last_offset_y;
	float m_yaw;
	float m_pitch;
	float m_speed;
#ifdef _DEBUG
	bool m_disabled;
#endif // _DEBUG

	glm::vec3 m_front;
	glm::vec3 m_right;
	glm::vec3 m_up;
	glm::mat4 m_projection;
	glm::mat4 m_view;
};

#endif // !_CAMERA_H
