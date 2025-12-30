#include "nav_mesh.h"

#include "core/managers/time.h"

#include <Detour/DetourNavMeshBuilder.h>
#include <Recast/Recast.h>

static bool create_nav_mesh(const rcPolyMesh& poly_mesh,
    const rcPolyMeshDetail& detail_mesh,
    const rcConfig& config,
    dtNavMesh* nav_mesh)
{
    dtNavMeshCreateParams params {};

    params.verts = poly_mesh.verts;
    params.vertCount = poly_mesh.nverts;
    params.polys = poly_mesh.polys;
    params.polyAreas = poly_mesh.areas;
    params.polyFlags = poly_mesh.flags;
    params.polyCount = poly_mesh.npolys;
    params.nvp = poly_mesh.nvp;
    params.detailMeshes = detail_mesh.meshes;
    params.detailVerts = detail_mesh.verts;
    params.detailVertsCount = detail_mesh.nverts;
    params.detailTris = detail_mesh.tris;
    params.detailTriCount = detail_mesh.ntris;

    params.walkableHeight = config.walkableHeight;
    params.walkableRadius = config.walkableRadius;
    params.walkableClimb = config.walkableClimb;
    params.cs = config.cs;
    params.ch = config.ch;
    params.buildBvTree = true;
    
    std::memcpy(params.bmin, poly_mesh.bmin, sizeof(params.bmin));
    std::memcpy(params.bmax, poly_mesh.bmax, sizeof(params.bmax));

    // Создание данных NavMesh
    uint8_t* nav_info = nullptr;
    int nav_size = 0;

    if (!dtCreateNavMeshData(&params, &nav_info, &nav_size))
        return false;

    // Инициализация NavMesh
    const dtStatus status = nav_mesh->init(nav_info, nav_size, DT_TILE_FREE_DATA);
    if (dtStatusFailed(status)) 
    {
        dtFreeNavMesh(nav_mesh);
        dtFree(nav_info);
        return false;
    }

    return true;
}

NavMesh::~NavMesh()
{
    dtFreeCrowd(m_agents);
}

bool NavMesh::generate(const std::vector<glm::vec3>& vertices,
	const std::vector<Face>& indices)
{
	rcConfig config {};
    mb_unique<rcContext> unique_context = create_unique<rcContext>();
    rcContext* context = unique_context.get();

	rcCalcBounds(glm::value_ptr(vertices[0]), 
		vertices.size() / 3,
		config.bmin, 
		config.bmax);

	//config.cs = 0.3f;        // Размер вокселя
	//config.ch = 0.2f;        // Высота вокселя
	//config.walkableSlopeAngle = 45.0f;
	//config.walkableHeight = 2.0f;
	//config.walkableClimb = 0.9f;
	//config.walkableRadius = 0.6f;
	//config.maxEdgeLen = 12.0f;
	//config.maxSimplificationError = 1.3f;
	//config.minRegionArea = 8.0f;
	//config.mergeRegionArea = 20.0f;
	//config.maxVertsPerPoly = 6;
	//config.detailSampleDist = 6.0f;
	//config.detailSampleMaxError = 1.0f;
    config.cs = 0.4f;  // Более крупные воксели для скорости
    config.ch = 0.3f;
    config.walkableSlopeAngle = 45.0f;
    config.walkableHeight = 4;              // Увеличиваем для скорости
    config.walkableClimb = 2;               // Уменьшаем для скорости
    config.walkableRadius = 2;
    config.maxEdgeLen = 20; // Увеличиваем для скорости
    config.maxSimplificationError = 2.0f;
    config.minRegionArea = 20;              // Увеличиваем для скорости
    config.mergeRegionArea = 50;
    config.maxVertsPerPoly = 6;
    config.detailSampleDist = 12.0f;
    config.detailSampleMaxError = 2.0f;
    
    rcCalcGridSize(config.bmin, config.bmax, config.cs, &config.width, &config.height);

	rcHeightfield height_field;
	if (!rcCreateHeightfield(context, height_field,
		config.width, config.height, 
        config.bmin, config.bmax,
        config.cs, config.ch))
	{
		return false;
	}

    std::vector<uint8_t> triangles(indices.size(), RC_WALKABLE_AREA);
    if (!rcRasterizeTriangles(context,
        glm::value_ptr(vertices[0]), 
        vertices.size() / 3,
        (int*)(&indices.data()->x),
        triangles.data(),
        indices.size(),
        height_field,
        config.walkableClimb))
    {
        return false;
    }

    // Фильтрация walkable surfaces
    rcFilterWalkableLowHeightSpans(context, config.walkableHeight, height_field);

    // Генерация регионов
    rcCompactHeightfield compact_height;
    if (!rcBuildCompactHeightfield(context,
        config.walkableHeight, 
        config.walkableClimb, 
        height_field, 
        compact_height))
    {
        return false;
    }

    // Эрозия walkable area
    if (!rcErodeWalkableArea(context, config.walkableRadius, compact_height))
        return false;

    // Построение дистанционного поля
    if (!rcBuildDistanceField(context, compact_height))
        return false;

    // Генерация регионов
    if (!rcBuildRegions(context, compact_height, 0, config.minRegionArea, config.mergeRegionArea))
        return false;

    // Создание контуров
    rcContourSet contour;
    if (!rcBuildContours(context, compact_height, config.maxSimplificationError, config.maxEdgeLen, contour))
        return false;

    // Построение полигональной сетки
    rcPolyMesh poly_mesh;
    if (!rcBuildPolyMesh(context, contour, config.maxVertsPerPoly, poly_mesh))
        return false;

    // Детализация
    rcPolyMeshDetail detail_mesh;
    if (!rcBuildPolyMeshDetail(context, poly_mesh, compact_height,
        config.detailSampleDist, config.detailSampleMaxError,
        detail_mesh)) 
    {
        return false;
    }

    m_nav_mesh = dtAllocNavMesh();
    if (!create_nav_mesh(poly_mesh, detail_mesh, config, m_nav_mesh))
    {
        dtFreeNavMesh(m_nav_mesh);
        return false;
    }

    m_agents = dtAllocCrowd();

    if (!m_agents->init(32, 0.6f, m_nav_mesh))
        return false;

    // Настройка параметров агента
    m_params.radius = 0.6f;
    m_params.height = 2.0f;
    m_params.maxAcceleration = 8.0f;
    m_params.maxSpeed = 3.5f;
    m_params.collisionQueryRange = m_params.radius * 12.0f;
    m_params.pathOptimizationRange = m_params.radius * 30.0f;
    m_params.updateFlags =
        DT_CROWD_ANTICIPATE_TURNS |
        DT_CROWD_OPTIMIZE_VIS |
        DT_CROWD_OPTIMIZE_TOPO |
        DT_CROWD_OBSTACLE_AVOIDANCE;
    m_params.obstacleAvoidanceType = 3;
    m_params.separationWeight = 2.0f;

    m_nav_query = m_agents->getNavMeshQuery();

	return true;
}

void NavMesh::update()
{
    if (!m_agents)
        return;

	m_agents->update(Time::get_delta_time(), nullptr);
}

bool NavMesh::request_agent_set_target(const int index, const glm::vec3& origin)
{
    dtPolyRef target {};
	float nearest[3] {};

	m_nav_query->findNearestPoly(
		glm::value_ptr(origin),
		m_agents->getQueryExtents(),
		m_agents->getFilter(0),
		&target,
		nearest
	);

	if (!target)
		return false;

	m_agents->requestMoveTarget(index, target, glm::value_ptr(origin));

	return true;
}

int NavMesh::add_agent(const glm::vec3& origin)
{
	return m_agents->addAgent(glm::value_ptr(origin), &m_params);
}

void NavMesh::remove_agent(const int index)
{
	m_agents->removeAgent(index);
}

const dtCrowdAgent* NavMesh::get_agent(const int index)
{
    return m_agents->getAgent(index);
}
