#include "material.h"

using namespace brf;

void Material::load(FileStreamReader& stream)
{
	m_name = stream.read_with_length();
	m_flags = stream.read<uint32_t>();
	m_shader = stream.read_with_length();
	m_diffuse_a = stream.read_with_length();
	m_diffuse_b = stream.read_with_length();
	m_bump = stream.read_with_length();
	m_environment = stream.read_with_length();
	m_specular = stream.read_or_default("none");

	m_specular_strength = stream.read<float>();
	m_r = stream.read<float>();
	m_g = stream.read<float>();
	m_b = stream.read<float>();
}
