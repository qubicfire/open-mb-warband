#ifndef _BFR_BODY_H
#define _BFR_BODY_H
#include "core/io/file_stream_reader.h"
#include "core/mb.h"
#include "utils/math/box3f.h"

namespace brf
{
	enum class BodyPartType : uint8_t
	{
		Manifold,
		Capsule,
		Sphere,
		Face,
	};

	class BodyPart
	{
		friend class Body;
	public:
		bool load(FileStreamReader& stream, std::string_view settings);
	private:
		Box3f calculate_bbox() const;
	private:
		BodyPartType m_type;
		int m_orientation;
		int m_flags;

		float m_radius;

		glm::vec3 m_center;
		glm::vec3 m_direction;
		//Box3f m_bbox;

		std::vector<glm::vec3> m_points;
		std::vector<std::vector<int>> m_faces;
	};

	class Body
	{
	public:
		bool load(FileStreamReader& stream);
	private:
		void calculate_bbox();
	private:
		std::string m_name;
		std::vector<BodyPart> m_parts;
		Box3f m_bbox;
	};
}

#endif // !_BFR_BODY_H