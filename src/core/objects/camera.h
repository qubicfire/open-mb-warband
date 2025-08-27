#ifndef _CAMERA_H
#define _CAMERA_H
#include <glm/ext/matrix_float4x4.hpp>

#include "object.h"

class Camera : public Object
{
	object_base_impl(Camera)
public:
	void client_start() override;
	void update() override;
	void update_view_matrix();

	glm::vec3 screen_point_to_ray();

	void set_fov(const float fov);

	const float get_fov() const;
	const glm::vec3& get_front() const;
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
	glm::vec3 m_up;
	glm::mat4 m_projection;
	glm::mat4 m_view;
};

#endif // !_CAMERA_H
