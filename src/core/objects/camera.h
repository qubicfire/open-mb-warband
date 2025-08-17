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

	void set_fov(const float fov);

	const float get_fov() const noexcept;
	const glm::vec3& get_front() const noexcept;
	const glm::vec3& get_up() const noexcept;
	const glm::mat4& get_projection() const noexcept;
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
	bool m_disabled;

	glm::vec3 m_front;
	glm::vec3 m_up;
	glm::mat4 m_projection;
};

#endif // !_CAMERA_H
