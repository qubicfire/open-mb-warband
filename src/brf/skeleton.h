#ifndef _BRF_SKELETON_H
#define _BRF_SKELETON_H
#include <glm/ext.hpp>
#include <glm/fwd.hpp>

#include "animation.h"

#include "utils/math/box3f.h"

namespace brf
{
	struct Bone
	{
		bool load(FileStreamReader& stream);

		glm::mat4 get_rotation_matrix() const;

		static glm::vec3 adjust_coordinate(const glm::vec3& point);
		static glm::quat adjust_coordinate(const glm::quat& point);
		static glm::mat4 adjust_coordinate(const glm::mat4& matrix);
		static glm::quat adjust_coordinate_half(const glm::quat& point);
		static glm::mat4 adjust_coordinate_half(const glm::mat4& matrix);
	public:
		std::string m_name;
		int m_attach;
		glm::vec3 m_x, m_y, m_z, m_t;
		std::vector<int> m_next;
	private:
		static void adjust_coordinate(Bone* bone);
		static void adjust_coordinate_half(Bone* bone);
	};

	class Skeleton
	{
	public:
		bool load(FileStreamReader& stream);

		std::vector<glm::mat4> get_bone_matrices(const AnimationFrame& frame);
		std::vector<glm::mat4> get_bone_matrices();
		std::vector<glm::mat4> get_bone_matrices_inverse();
	private:
		void set_bone_matrices(const AnimationFrame& frame,
			std::vector<glm::mat4>& output,
			const glm::mat4& identity,
			int index);
		void set_bone_matrices(std::vector<glm::mat4>& output,
			const glm::mat4& identity,
			int index);
		void set_bone_matrices_inverse(std::vector<glm::mat4>& output,
			const glm::mat4& identity,
			int index);
	private:
		std::string m_name;
		uint32_t m_flags;
		int m_root;

		std::vector<Bone> m_bones;
		Box3f m_bbox;
	};
}

#endif // !_BRF_SKELETON_H
