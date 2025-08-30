#include "object.h"

#include "rigidbody.h"

#include <Jolt/Physics/Collision/Shape/MeshShape.h>
#include <Jolt/Physics/Collision/Shape/ConvexHullShape.h>
#include <Jolt/Physics/Collision/Shape/HeightFieldShape.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>

#include "core/managers/physics.h"

using namespace JPH;

static Array<Vec3> convert_vertices(const std::vector<glm::vec3>& vertices)
{
	Array<Vec3> new_vertices {};
	new_vertices.resize(vertices.size());

	for (int i = 0; i < vertices.size(); i++)
	{
		const glm::vec3& vertex = vertices[i];

		new_vertices[i] = Vec3(vertex.x, vertex.y, vertex.z);
	}

	return new_vertices;
}

static Vec3 convert_glm_vec3(const glm::vec3& v)
{
	return Vec3(v.x, v.y, v.z);
}

RigidBody::~RigidBody()
{
	g_physics->remove_body(m_body);
}

bool RigidBody::create_body(Object* object,
	const std::vector<glm::vec3>& vertices,
	const std::vector<uint32_t>& indices,
	const MotionType type,
	const ActivationType state,
	const int collision_layer)
{
	m_object = object;

	VertexList jolt_vertices {};
	jolt_vertices.reserve(vertices.size());

	for (const auto& vertex : vertices)
		jolt_vertices.emplace_back(vertex.x, vertex.y, vertex.z);

	IndexedTriangleList jolt_indices {};
	jolt_indices.reserve(indices.size() / 3);

	for (int i = 0; i < indices.size(); i += 3)
		jolt_indices.emplace_back(indices[i], indices[i + 1], indices[i + 2], 0);

	MeshShapeSettings mesh_settings(jolt_vertices, jolt_indices);
	ShapeSettings::ShapeResult result = mesh_settings.Create();
	if (result.HasError())
	{
		log_alert(result.GetError().c_str());
		return false;
	}

	ShapeRefC shape = result.Get();

	BodyCreationSettings settings(shape,
		convert_glm_vec3(object->get_origin()),
		Quat::sIdentity(),
		type,
		collision_layer);

	m_body = g_physics->create_body(settings, state);
	m_body->SetUserData(reinterpret_cast<uint64_t>(m_object));

	return true;
}

JPH::Body* RigidBody::get_body() const
{
	return m_body;
}
