#ifndef _BRF_SKELETON_H
#define _BRF_SKELETON_H
#include <glm/ext.hpp>

#include "core/mb.h"
#include "core/io/file_stream_reader.h"
#include "utils/math/box3f.h"

namespace brf
{
	struct Bone
	{
		bool load(FileStreamReader& stream);

		std::string m_name;
		int m_attach;
		glm::vec3 m_x, m_y, m_z, m_t;
		std::vector<int> m_next;
	public:
		static glm::vec3 adjust_coordinate(const glm::vec3& point);
		static glm::vec4 adjust_coordinate(const glm::vec4& point);
		static glm::vec4 adjust_coordinate_half(const glm::vec4& point);
	private:
		static void adjust_coordinate(Bone* bone);
		static void adjust_coordinate_half(Bone* bone);
	};

	class Skeleton
	{
	public:
		bool load(FileStreamReader& stream);
	private:
		std::string m_name;
		uint32_t m_flags;
		int m_root;

		std::vector<Bone> m_bones;
		Box3f m_bbox;
	};
}

#endif // !_BRF_SKELETON_H
