#include "texture.h"

using namespace brf;

bool Texture::load(FileStreamReader& stream)
{
    m_name = stream.read_with_length();
    m_flags = stream.read<uint32_t>();

    return true;
}
