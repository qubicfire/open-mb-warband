#ifndef _BRF_ANIMATION_H
#define _BRF_ANIMATION_H
#include "core/mb.h"
#include "core/io/file_stream_reader.h"

namespace brf
{
	struct AnimationFrame
	{
		int m_index;
		glm::vec3 m_transform;
		std::vector<glm::vec4> m_rotations;
		std::vector<bool> m_was_implicit;
	};

	class Animation
	{
	public:
		bool load(FileStreamReader& stream);
	private:
		std::string m_name;
		uint32_t m_bones_count;
		std::vector<AnimationFrame> m_frames;
	};
}

#endif // !_BRF_ANIMATION_H
