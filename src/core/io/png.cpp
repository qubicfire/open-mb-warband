#include <stb_image/stb_image.h>
#include "core/platform/opengl/opengl.h"

#include "file_descriptor.h"

#include "png.h"

bool PNGTexture::load(const std::string& path, mbcore::TextureProperties& properties)
{
    FileDescriptor file = FileDescriptor::open(path);
    if (file.m_info == nullptr)
        return false;

    int width, height, channels;
    uint8_t* texture = stbi_load_from_memory(file.m_info.get(),
        file.m_length,
        &width, &height, &channels, 3);

    properties.m_width = static_cast<uint32_t>(width);
    properties.m_height = static_cast<uint32_t>(height);
    properties.m_size = file.m_length;
    properties.m_texture = std::move(Unique<uint8_t[]>{ texture });
    properties.m_start = properties.m_texture.get();
    // TODO: Implement DirectX
    properties.m_format = channels == 3 ? GL_RGB : GL_RGBA;

    return true;
}
