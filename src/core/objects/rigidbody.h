#ifndef _RIGIDBODY_H
#define _RIGIDBODY_H
#include <Jolt/Jolt.h>

// Jolt includes
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/EActivation.h>
#include <Jolt/Physics/Body/MotionType.h>

class Object;

using ActivationType = JPH::EActivation;
using MotionType = JPH::EMotionType;

struct Face
{
	uint32_t x;
	uint32_t y;
	uint32_t z;
};

class RigidBody
{
public:
	~RigidBody();

	bool create_body(Object* object,
		const std::vector<glm::vec3>& vertices,
		const std::vector<Face>& faces,
		const MotionType type,
		const ActivationType state,
		const int collision_layer);

	JPH::Body* get_body() const;
private:
	JPH::Body* m_body;
};

#endif // !_RIGIDBODY_H
