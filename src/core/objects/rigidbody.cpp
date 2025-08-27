#include "object.h"

#include "rigidbody.h"

#include <Jolt/Physics/Collision/Shape/ConvexHullShape.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>

#include "core/managers/physics.h"

#include "utils/vhacd.h"

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

#include <glm/ext.hpp>

bool RigidBody::create_body(Object* object,
	const std::vector<glm::vec3>& vertices,
	const std::vector<uint32_t>& indices,
	const MotionType type,
	const ActivationState state,
	const int collision_layer)
{
	//VHACD::VHACDImpl decompositor;
	//VHACD::IVHACD::Parameters params;

	//std::vector<VHACD::Vertex> vhacd_vertex {};
	//vhacd_vertex.reserve(vertices.size());

	//for (const auto& vertex : vertices)
	//	vhacd_vertex.emplace_back(vertex.x, vertex.y, vertex.z);

	//std::vector<VHACD::Triangle> vhacd_triangles {};
	//vhacd_triangles.reserve(indices.size());

	//for (uint32_t i = 0; i < indices.size(); i += 3)
	//	vhacd_triangles.emplace_back(indices[i], indices[i + 1], indices[i + 2]);

	//bool success = decompositor.Compute(vhacd_vertex, vhacd_triangles, params);

	//if (!success)
	//{
	//	log_alert("Failed to decomposite convex hull shape. " 
	//		"Unable to process creation of physics shape");

	//	return false;
	//}

	m_object = object;

	ConvexHullShapeSettings shape_settings(convert_vertices(vertices));
	shape_settings.mHullTolerance = 0.01f;

	ShapeSettings::ShapeResult result = shape_settings.Create();
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
