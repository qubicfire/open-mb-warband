#ifndef _CAMERA_H
#define _CAMERA_H
#include <glm/ext/matrix_float4x4.hpp>

#include "object.h"

#include "core/managers/physics.h"

struct Frustum
{
	enum
	{
		Left,
		Right,
		Bottom,
		Top,
		Near,
		Far,
		PlaneCount
	};

	bool is_visible(const AABB& aabb) const;

	glm::vec4 m_planes[PlaneCount];
};

class Camera : public Object
{
	object_base(Camera)
public:
	void start_client() override;
	void update() override;
	void update_view_matrix();

	Frustum create_frustum();
	Ray screen_point_to_ray();

	void set_fov(const float fov);

	const float get_fov() const;
	const glm::vec3& get_front() const;
	const glm::vec3& get_right() const;
	const glm::vec3& get_up() const;
	const glm::mat4& get_projection() const;
	const glm::mat4& get_inverse_projection() const;
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
	glm::mat4 m_inverse_projection;
	glm::mat4 m_view;
};

#endif // !_CAMERA_H
