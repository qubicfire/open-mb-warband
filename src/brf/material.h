#ifndef _BRF_MATERIAL_H
#define _BRF_MATERIAL_H
#include "core/mb.h"
#include "core/io/file_stream_reader.h"

namespace brf
{
	class Material
	{
	public:
		void load(FileStreamReader& stream);
	private:
		std::string m_name;
		uint32_t m_flags;
		std::string m_shader;
		std::string m_diffuse_a;
		std::string m_diffuse_b;
		std::string m_bump;
		std::string m_environment;
		std::string m_specular;
		float m_specular_strength;
		float m_r;
		float m_g;
		float m_b;
	};
}

#endif // !_BRF_MATERIAL_H
