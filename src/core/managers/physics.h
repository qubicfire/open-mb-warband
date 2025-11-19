#ifndef _PHYSICS_H
#define _PHYSICS_H
#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>

#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>

#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>
#include <Jolt/Physics/Collision/RayCast.h>
#include <Jolt/Physics/Collision/CastResult.h>

#ifdef _DEBUG
	#include <jolt/Renderer/DebugRenderer.h>
#endif // _DEBUG

#include "core/mb.h"
#include "core/mb_type_traits.h"

namespace CollisionLayers
{
	static constexpr JPH::ObjectLayer STATIC = 0;
	static constexpr JPH::ObjectLayer DYNAMIC = 1;
	static constexpr JPH::ObjectLayer NUM_LAYERS = 2;
}

namespace BroadPhaseLayers
{
	static constexpr JPH::BroadPhaseLayer STATIC(0);
	static constexpr JPH::BroadPhaseLayer DYNAMIC(1);
	static constexpr uint32_t NUM_LAYERS(2);
};

class Object;

struct Ray
{
	Ray() = default;

	Ray(const glm::vec3& start,
		const glm::vec3& direction,
		const float distance)
		: m_start(start)
		, m_direction(direction)
		, m_distance(distance)
	{ }

	glm::vec3 m_start;
	glm::vec3 m_direction;
	float m_distance;
};

struct RayCastInfo
{
	glm::vec3 m_hit_point;
	Object* m_object;
};

class Physics
{
	friend class RigidBody;

	class PhysicsLayerInterfaceImpl final : public JPH::BroadPhaseLayerInterface
	{
	public:
		PhysicsLayerInterfaceImpl();

		uint32_t GetNumBroadPhaseLayers() const override;
		JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer layer) const override;
#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
		const char* GetBroadPhaseLayerName(JPH::BroadPhaseLayer layer) const override;
#endif
	private:
		JPH::BroadPhaseLayer m_object_to_broadphase[CollisionLayers::NUM_LAYERS];
	};

	/// Class that determines if an object layer can collide with a broadphase layer
	struct PhysicsBroadPhaseLayerFilterImpl final : public JPH::ObjectVsBroadPhaseLayerFilter
	{
		bool ShouldCollide(JPH::ObjectLayer a,
			JPH::BroadPhaseLayer b) const override;
	};

	struct PhysicsContactListener final : public JPH::ContactListener
	{
		JPH::ValidateResult	OnContactValidate(const JPH::Body& body_a,
			const JPH::Body& body_b,
			JPH::RVec3Arg base_offset,
			const JPH::CollideShapeResult& collision_result) override;

		void OnContactAdded(const JPH::Body& body_a,
			const JPH::Body& body_b,
			const JPH::ContactManifold& manifold,
			JPH::ContactSettings& contact_settings) override;

		void OnContactPersisted(const JPH::Body& body_a,
			const JPH::Body& body_b,
			const JPH::ContactManifold& manifold,
			JPH::ContactSettings& ioSettings) override;

		void OnContactRemoved(const JPH::SubShapeIDPair& sub_shape_pair) override;
	};

	struct PhysicsActivationListener final : public JPH::BodyActivationListener
	{
		void OnBodyActivated(const JPH::BodyID& body,
			uint64_t user_data) override;

		void OnBodyDeactivated(const JPH::BodyID& body,
			uint64_t user_data) override;
	};

	struct PhysicsObjectLayerFilter final : public JPH::ObjectLayerFilter
	{
		bool ShouldCollide(JPH::ObjectLayer layer) const override;
	};

	struct PhysicsBroadPhaseLayerFilter final : public JPH::BroadPhaseLayerFilter
	{
		bool ShouldCollide(JPH::BroadPhaseLayer layer) const override;
	};
public:
	~Physics();

	void initialize();
	void update();

	static bool raycast(const glm::vec3& origin, 
		const glm::vec3& direction,
		const float distance,
		RayCastInfo& info);
	static bool raycast(const Ray& ray, RayCastInfo& info);

#ifdef _DEBUG
	static inline bool m_is_debug_draw = false;
#endif // _DEBUG
private:
	JPH::Body* create_body(const JPH::BodyCreationSettings& settings, 
		const JPH::EActivation state);
	void remove_body(JPH::Body* body);
private:
	static constexpr auto MAX_BODIES = 1024;
	static constexpr auto NUM_BODY_MUTEXES = 0;
	static constexpr auto MAX_BODY_PAIRS = 1024;
	static constexpr auto MAX_CONTACT_CONSTRAINTS = 1024;

	JPH::PhysicsSystem m_physics_system;
	JPH::JobSystemThreadPool m_job_system;

	PhysicsLayerInterfaceImpl m_broad_phase_layer_interface;
	PhysicsBroadPhaseLayerFilterImpl m_broad_phase_layer_filter;
	JPH::ObjectLayerPairFilter m_object_pair_filter;
	PhysicsActivationListener m_activation_listener;
	PhysicsContactListener m_contact_listener;
	JPH::TempAllocatorMalloc m_temp_allocator;

#ifdef _DEBUG
	JPH::DebugRenderer* m_renderer;
#endif // _DEBUG
};

create_global_class(Physics, physics)

#endif // !_PHYSICS_H
