#include "utils/pugixml.hpp"

#include "text_builder_3d.h"

using namespace mbcore;

bool TextBuilder3D::load()
{
    pugi::xml_document file {};
    pugi::xml_parse_result result = file.load_file("Data/font_data.xml");

    if (!result)
    {
        log_alert(result.description());
        return false;
    }

    pugi::xml_node root = file.child("FontData");
    m_font_settings.m_width = root.attribute("width").as_int();
    m_font_settings.m_height = root.attribute("height").as_int();
    m_font_settings.m_padding = root.attribute("padding").as_int();
    m_font_settings.m_size = root.attribute("font_size").as_int();
    m_font_settings.m_scale = root.attribute("font_scale").as_int();
    m_font_settings.m_line_spacing = root.attribute("line_spacing").as_float();

    pugi::xml_node details = root.child("FontDetails");

    for (const auto& detail : details)
    {
        CharacterSettings settings {};
        int code = detail.attribute("code").as_int();
        settings.m_page = detail.attribute("page").as_int();
        settings.m_x = detail.attribute("u").as_int();
        settings.m_y = detail.attribute("v").as_int();
        settings.m_width = detail.attribute("w").as_float();
        settings.m_height = detail.attribute("h").as_float();
        settings.m_pre_shift = detail.attribute("preshift").as_float();
        settings.m_post_shift = detail.attribute("postshift").as_float();
        settings.m_y_adjust = detail.attribute("yadjust").as_int();

        m_chars.emplace(code, settings);
    }

    // TODO: TEMP PATH. CHANGE IT LATER
    m_font = Texture2D::create("test/font.dds", Texture2D::DDS);

    return true;
}

Unique<VertexArray> TextBuilder3D::construct(const std::string& text)
{
    constexpr int TRIANGLES_PER_EACH_SYMBOL = 6;
    struct TextVertex
    {
        glm::vec3 m_origin;
        glm::vec2 m_texture;
    };

    std::vector<TextVertex> vertices {};
    vertices.reserve(text.size() * TRIANGLES_PER_EACH_SYMBOL);

    //
    // v0 -> |-------| <- v1
    //       |      ||
    //       |     | |
    //       |    |  |
    //       |   |   |
    //       |  |    |
    //       | |     |   
    // v2 -> |-------| <- v4
    // 
    // First triangle: (v0; v1; v2) including uv texture coordinates
    // Second triangle: (v1; v2; v4) including uv texture coordinates
    // That logic will be done on every character

    float inverse_width = 1.0f / m_font_settings.m_width;
    float inverse_height = 1.0f / m_font_settings.m_height;

    //    -1   1
    //   - - - - -   # Divide text size by half and get the left start 
    //  |w|o|r|l|d|  # origin for the first char and increasing offset
    //   - - - - -   # by 1.0f. Repeat until chars done
    //  -2   ^   2
    //       |
    // Should be center of the word (offset would be 0.0 in there)

    float start_offset_x = -static_cast<float>(text.size() / 2);

    for (const auto c : text)
    {
        const auto& settings = m_chars[c];

        // without constant on the end, texture would contain lines from another symbol
        const float u = ((settings.m_x - m_font_settings.m_padding) * inverse_width) + 0.001f;
        const float v = ((settings.m_y - m_font_settings.m_padding) * inverse_height) + 0.001f;

        const float u_offset_x = (settings.m_width - m_font_settings.m_padding) * inverse_width;
        const float v_offset_y = (settings.m_height - m_font_settings.m_padding) * inverse_height;

        const float aspect_y = settings.m_height - settings.m_y;
        const float y_adjust = inverse_height * (settings.m_y_adjust - aspect_y);

        if (settings.m_pre_shift != 0)
            start_offset_x = start_offset_x - settings.m_pre_shift / m_font_settings.m_line_spacing;

        vertices.push_back({ {start_offset_x, 0.5f + y_adjust, 0.5f}, {u, v} });
        vertices.push_back({ {start_offset_x, -0.5f - y_adjust, 0.5f}, {u, v_offset_y} });
        vertices.push_back({ {start_offset_x + 1.0f, 0.5f + y_adjust, 0.5f}, {u_offset_x, v} });
        vertices.push_back({ {start_offset_x + 1.0f, 0.5f + y_adjust, 0.5f}, {u_offset_x, v} });
        vertices.push_back({ {start_offset_x, -0.5f - y_adjust, 0.5f}, {u, v_offset_y} });
        vertices.push_back({ {start_offset_x + 1.0f, -0.5f - y_adjust, 0.5f}, {u_offset_x, v_offset_y} });
        
        if (settings.m_post_shift != 0)
            //start_offset_x = start_offset_x + (1.0f - (1.0f / settings.m_post_shift));
            start_offset_x = start_offset_x + 0.5f + settings.m_post_shift / m_font_settings.m_line_spacing;
        else
            start_offset_x = start_offset_x + 1.0f;
    }

    Unique<VertexArray> array = VertexArray::create();
    Unique<VertexBuffer> buffer = VertexBuffer::create(vertices, BufferFlags::Dynamic);

    array->link(0, VertexType::Float3, cast_offset(TextVertex, m_origin));
    array->link(1, VertexType::Float2, cast_offset(TextVertex, m_texture));

    array->set_vertex_buffer(buffer);

    array->unbind();
    return array;
}

Texture2D* TextBuilder3D::get_texture() const
{
    return m_font;
}
