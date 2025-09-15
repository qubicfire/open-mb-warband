#include "core/objects/object.h"

#ifdef _DEBUG
	#include "core/platform/opengl/opengl.h"
	#include "core/graphics/renderer.h"
	#include "core/graphics/shader.h"
	#include "core/objects/camera.h"
	#include "core/managers/assets.h"
#endif // _DEBUG

#include "physics.h"

using namespace JPH;

Physics::PhysicsLayerInterfaceImpl::PhysicsLayerInterfaceImpl()
{
	// Create a mapping table from object to broad phase layer
	m_object_to_broadphase[CollisionLayers::STATIC] = BroadPhaseLayers::STATIC;
	m_object_to_broadphase[CollisionLayers::DYNAMIC] = BroadPhaseLayers::DYNAMIC;
}

uint32_t Physics::PhysicsLayerInterfaceImpl::GetNumBroadPhaseLayers() const
{
	return BroadPhaseLayers::NUM_LAYERS;
}

BroadPhaseLayer Physics::PhysicsLayerInterfaceImpl::GetBroadPhaseLayer(ObjectLayer layer) const
{
	JPH_ASSERT(layer < CollisionLayers::NUM_LAYERS);
	return m_object_to_broadphase[layer];
}

const char* Physics::PhysicsLayerInterfaceImpl::GetBroadPhaseLayerName(BroadPhaseLayer layer) const
{
	switch ((BroadPhaseLayer::Type)layer)
	{
	case (BroadPhaseLayer::Type)BroadPhaseLayers::STATIC: return "STATIC";
	case (BroadPhaseLayer::Type)BroadPhaseLayers::DYNAMIC:	return "DYNAMIC";
	default: JPH_ASSERT(false); return "INVALID";
	}
}

bool Physics::PhysicsBroadPhaseLayerFilterImpl::ShouldCollide(ObjectLayer a,
	BroadPhaseLayer b) const
{
	switch (a)
	{
	case CollisionLayers::STATIC:
		return b == BroadPhaseLayers::DYNAMIC;
	case CollisionLayers::DYNAMIC:
		return true;
	default:
		JPH_ASSERT(false);
		return false;
	}
}

ValidateResult Physics::PhysicsContactListener::OnContactValidate(const Body& body_a, 
	const Body& body_b,
	RVec3Arg base_offset,
	const CollideShapeResult& collision_result)
{
	// Allows you to ignore a contact before it is created (using layers to not make objects collide is cheaper!)
	return ValidateResult::AcceptAllContactsForThisBodyPair;
}

void Physics::PhysicsContactListener::OnContactAdded(const Body& body_a,
	const Body& body_b,
	const ContactManifold& manifold,
	ContactSettings& settings)
{
}

void Physics::PhysicsContactListener::OnContactPersisted(const Body& body_a,
	const Body& body_b,
	const ContactManifold& manifold,
	ContactSettings& settings)
{
}

void Physics::PhysicsContactListener::OnContactRemoved(const SubShapeIDPair& sub_shape_pair)
{
}

void Physics::PhysicsActivationListener::OnBodyActivated(const BodyID& body,
	uint64_t user_data)
{
	Object* object = reinterpret_cast<Object*>(user_data);

	log_print("body got activated: ", object->get_id());
}

void Physics::PhysicsActivationListener::OnBodyDeactivated(const BodyID& body, 
	uint64_t user_data)
{
}

struct GLBatch
{
	GLuint m_vao;
	GLuint m_vbo;
	int m_triangles;
};

class OpenGLBatchRefTarget : public JPH::RefTargetVirtual
{
public:
	OpenGLBatchRefTarget(GLBatch* batch) : m_batch(batch) {}

	// Обязательная реализация виртуального метода
	virtual void AddRef() override { /* Jolt сам управляет счетчиком */ }
	virtual void Release() override { /* Jolt сам управляет счетчиком */ }

	GLBatch* GetBatch() { return m_batch; }

private:
	GLBatch* m_batch;
};

#ifdef _DEBUG
struct OpenGLDebugRenderer : public DebugRenderer
{
	std::vector<mb_unique<GLBatch>> m_batches;

	~OpenGLDebugRenderer()
	{
		for (const auto& batch : m_batches)
		{
			glDeleteBuffers(1, &batch->m_vbo);
			glDeleteVertexArrays(1, &batch->m_vao);
		}
	}

	void DrawLine(RVec3Arg from,
		RVec3Arg to, 
		ColorArg color) override
	{
		glColor3f(color.r, color.g, color.b);
		glBegin(GL_LINES);
		glVertex3f(from.GetX(), from.GetY(), from.GetZ());
		glVertex3f(to.GetX(), to.GetY(), to.GetZ());
		glEnd();
	}
	void DrawTriangle(RVec3Arg v1,
		RVec3Arg v2,
		RVec3Arg v3, 
		ColorArg color,
		ECastShadow cast_shadow) override
	{
		glColor3f(color.r, color.g, color.b);
		glBegin(GL_TRIANGLES);
		glVertex3f(v1.GetX(), v1.GetY(), v1.GetZ());
		glVertex3f(v2.GetX(), v2.GetY(), v2.GetZ());
		glVertex3f(v3.GetX(), v3.GetY(), v3.GetZ());
		glEnd();
	}
	Batch CreateTriangleBatch(const Triangle* triangles,
		int triangle_count) override
	{
		mb_unique<GLBatch> new_batch = create_unique<GLBatch>();
		new_batch->m_triangles = triangle_count;

		// Create GPU buffers
		glGenVertexArrays(1, &new_batch->m_vao);
		glGenBuffers(1, &new_batch->m_vbo);

		// Bind and fill the buffers
		glBindVertexArray(new_batch->m_vao);
		glBindBuffer(GL_ARRAY_BUFFER, new_batch->m_vbo);

		// Upload the triangle data. inTriangles is an array of JPH::DebugRenderer::Triangle
		// Each Triangle has JPH::Float3 vertices[3] and JPH::Color color
		// We will only upload the vertex positions (3 floats per vertex)
		std::vector<glm::vec3> vertices {};
		vertices.reserve(triangle_count * 3);
		for (int i = 0; i < triangle_count; ++i) 
		{
			const auto& tri = triangles[i];
			// Convert Jolt's Float3 to glm::vec3 and push each vertex
			vertices.emplace_back(tri.mV[0].mPosition.x, tri.mV[0].mPosition.y, tri.mV[0].mPosition.z);
			vertices.emplace_back(tri.mV[1].mPosition.x, tri.mV[1].mPosition.y, tri.mV[1].mPosition.z);
			vertices.emplace_back(tri.mV[2].mPosition.x, tri.mV[2].mPosition.y, tri.mV[2].mPosition.z);
		}

		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

		glBindVertexArray(0); // Unbind VAO

		// Store the batch in our vector. The Batch's 'id' will be its index.
		JPH::DebugRenderer::Batch batch(new OpenGLBatchRefTarget(new_batch.get()));
		m_batches.push_back(std::move(new_batch));
		return batch;
	}
	Batch CreateTriangleBatch(const Vertex* vertices, 
		int vertex_count, 
		const uint32* indices,
		int index_count) override
	{
		return Batch();
	}
	void DrawGeometry(RMat44Arg matrix, 
		const AABox& world_space_bounds,
		float lod_scale, 
		ColorArg model_color,
		const GeometryRef& geometry, 
		ECullMode cull_mode,
		ECastShadow cast_shadow,
		EDrawMode draw_mode) override
	{
		Shader* shader = g_assets->get_shader("main");

		const Batch& batch = geometry->mLODs[0].mTriangleBatch; // Get the first LOD
		OpenGLBatchRefTarget* target = static_cast<OpenGLBatchRefTarget*>(batch.GetPtr());
		const GLBatch* gl_batch = target->GetBatch();

		glCullFace(cull_mode == ECullMode::CullBackFace ? GL_BACK : GL_FRONT);
		glPolygonMode(GL_FRONT_AND_BACK, draw_mode == EDrawMode::Solid ? GL_FILL : GL_LINE);

		glm::mat4 model = glm::mat4(
			matrix(0, 0), matrix(1, 0), matrix(2, 0), matrix(3, 0),
			matrix(0, 1), matrix(1, 1), matrix(2, 1), matrix(3, 1),
			matrix(0, 2), matrix(1, 2), matrix(2, 2), matrix(3, 2),
			matrix(0, 3), matrix(1, 3), matrix(2, 3), matrix(3, 3) 
		);

		Camera* camera = Renderer::get_camera();

		shader->bind();

		shader->set_mat4("u_view", camera->get_view());
		shader->set_mat4("u_projection", camera->get_projection());
		shader->set_mat4("u_model", model);

		glBindVertexArray(gl_batch->m_vao);
		glDrawArrays(GL_TRIANGLES, 0, gl_batch->m_triangles * 3);
		glBindVertexArray(0);

		glDisable(GL_CULL_FACE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	void DrawText3D(RVec3Arg origin, 
		const string_view& string, 
		ColorArg color,
		float height) override
	{
	}
};
#endif // _DEBUG

Physics::~Physics()
{
	UnregisterTypes();
	
#ifdef _DEBUG
	delete DebugRenderer::sInstance;
	DebugRenderer::sInstance = nullptr;
#endif

	delete Factory::sInstance;
	Factory::sInstance = nullptr;
}

static void jolt_trace(const char* format, ...)
{
	// Format the message
	va_list list;
	va_start(list, format);
	char buffer[1024];
	vsnprintf(buffer, sizeof(buffer), format, list);
	va_end(list);

	// Print to the TTY
	log_print(buffer);
}

// Callback for asserts, connect this to your own assert handler if you have one
static bool jolt_assert(const char* expression,
	const char* message, 
	const char* file, 
	uint32_t line)
{
	log_print("%s:%d: (%s) %s", file, line, expression, message);
	return true;
};

void Physics::load()
{
	RegisterDefaultAllocator();

	Trace = jolt_trace;
	JPH_IF_ENABLE_ASSERTS(AssertFailed = jolt_assert);

	Factory::sInstance = new Factory();

	RegisterTypes();

	m_job_system.Init(cMaxPhysicsJobs,
		cMaxPhysicsBarriers,
		std::thread::hardware_concurrency() - 1);

	m_physics_system.Init(MAX_BODIES,
		NUM_BODY_MUTEXES,
		MAX_BODY_PAIRS,
		MAX_CONTACT_CONSTRAINTS,
		m_broad_phase_layer_interface,
		m_broad_phase_layer_filter,
		m_object_pair_filter);

	m_physics_system.SetBodyActivationListener(&m_activation_listener);
	m_physics_system.SetContactListener(&m_contact_listener);

#ifdef _DEBUG
	DebugRenderer::sInstance = new OpenGLDebugRenderer();
#endif
}

void Physics::update()
{
	constexpr float DELTA_TIME = 1.0f / 60.0f;

	m_physics_system.Update(DELTA_TIME, 1, &m_temp_allocator, &m_job_system);

#ifdef _DEBUG
	if (m_is_debug_draw)
	{
		BodyManager::DrawSettings settings;
		settings.mDrawShapeWireframe = true;
		m_physics_system.DrawBodies(settings, DebugRenderer::sInstance);
	}
#endif // _DEBUG
}

static Vec3 convert_glm_vec3(const glm::vec3& v)
{
	return Vec3(v.x, v.y, v.z);
}

static glm::vec3 convert_jolt_vec3(const Vec3& v)
{
	return glm::vec3(v.GetX(), v.GetY(), v.GetZ());
}

bool Physics::raycast(const glm::vec3& origin, 
	const glm::vec3& direction, 
	const float distance,
	RayCastInfo& info)
{
	PhysicsSystem& physics_system = g_physics->m_physics_system;

	RRayCast ray;
	ray.mOrigin = convert_glm_vec3(origin);
	ray.mDirection = convert_glm_vec3(direction * distance);
	RayCastResult result;

	Physics::PhysicsBroadPhaseLayerFilter broad_phase_layer_filter;
	Physics::PhysicsObjectLayerFilter object_layer_filter;

	bool hit = physics_system.GetNarrowPhaseQuery().CastRay(ray,
		result,
		broad_phase_layer_filter,
		object_layer_filter);

	if (hit)
	{
		info.m_hit_point = convert_jolt_vec3(ray.GetPointOnRay(result.mFraction));

		BodyLockRead lock(physics_system.GetBodyLockInterface(), result.mBodyID);
		if (lock.Succeeded())
		{
			info.m_object = reinterpret_cast<Object*>(
				lock.GetBody().GetUserData()
			);
		}
	}

	return hit;
}

bool Physics::raycast(const Ray& ray, RayCastInfo& info)
{
	return raycast(ray.m_start, ray.m_direction, ray.m_distance, info);
}

Body* Physics::create_body(const BodyCreationSettings& settings,
	const EActivation state)
{
	BodyInterface& body_interface = m_physics_system.GetBodyInterface();
	Body* body = body_interface.CreateBody(settings);
	body_interface.AddBody(body->GetID(), state);

	return body;
}

void Physics::remove_body(JPH::Body* body)
{
	BodyInterface& body_interface = m_physics_system.GetBodyInterface();
	const BodyID& id = body->GetID();

	body_interface.RemoveBody(id);
	body_interface.DestroyBody(id);
}

bool Physics::PhysicsObjectLayerFilter::ShouldCollide(ObjectLayer layer) const
{
	return layer == CollisionLayers::STATIC || layer == CollisionLayers::DYNAMIC;
}

bool Physics::PhysicsBroadPhaseLayerFilter::ShouldCollide(BroadPhaseLayer layer) const
{
	return layer == BroadPhaseLayers::STATIC || layer == BroadPhaseLayers::DYNAMIC;
}
