#include "TextNode.hpp"
#include "RenderingServer.hpp"

#include <glm/gtx/transform.hpp>
#include <glm/mat4x4.hpp>

TextNode::TextNode(RenderingServer& rs, const std::shared_ptr<FreeTypeFont>& font) : RectangularGUINode(rs), font(font) {}

void TextNode::set_text(std::string_view new_text) {
    // Do nothing if we don't have a font.
    if (font == nullptr) {
        return;
    }

    chars.clear();

    // Allocate the buffers.
    std::vector<glm::vec3> vertices;
    vertices.reserve(new_text.length() * 6);
    std::vector<glm::vec2> uvs;
    uvs.reserve(new_text.length() * 6);

    float baseline_x {0.0f};
    for (char c : new_text) {
        const FreeTypeFont::FontChar& font_char = font->get_font_char(c);
        chars.push_back(std::ref(font_char));

        const float left_x = baseline_x + font_char.position.x;
        const float right_x = left_x + font_char.texture.get_size().x;
        const float top_y = static_cast<float>(font_char.position.y);
        const float bottom_y = static_cast<float>(font_char.position.y) - static_cast<float>(font_char.texture.get_size().y);

        if (right_x > max_x) max_x = right_x;
        if (left_x < min_x) min_x = left_x;
        if (top_y > max_y) max_y = top_y;
        if (bottom_y < min_y) min_y = bottom_y;

        vertices.emplace_back(left_x, top_y, 0.0f);
        vertices.emplace_back(left_x, bottom_y, 0.0f);
        vertices.emplace_back(right_x, bottom_y, 0.0f);
        vertices.emplace_back(left_x, top_y, 0.0f);
        vertices.emplace_back(right_x, bottom_y, 0.0f);
        vertices.emplace_back(right_x, top_y, 0.0f);

        uvs.emplace_back(0.0f, 0.0f);
        uvs.emplace_back(0.0f, 1.0f);
        uvs.emplace_back(1.0f, 1.0f);
        uvs.emplace_back(0.0f, 0.0f);
        uvs.emplace_back(1.0f, 1.0f);
        uvs.emplace_back(1.0f, 0.0f);

        baseline_x += font_char.advance / 64.0f;
    }

    mesh = std::make_optional<Mesh>();
    mesh->set_vertices(vertices);
    mesh->set_uvs(uvs);
}

void TextNode::draw() {
    draw_children();

    const glm::vec2 window_size {rs.get_window().get_window_size()};
    const glm::vec2 pixels_to_opengl_scale {2.0f / window_size};
    const glm::vec2 relative_origin_in_pixels {get_origin()};
    const glm::vec3 relative_origin_opengl {relative_origin_in_pixels * pixels_to_opengl_scale, 0.0};
    const glm::vec3 absolute_position_opengl {
        get_absolute_transform().to_opengl_position(rs.get_window().get_window_size()) - relative_origin_opengl
    };

    const glm::mat4 mvp {glm::translate(absolute_position_opengl) * glm::scale(glm::vec3(pixels_to_opengl_scale, 1.0f))};

    rs.get_shader_holder().get_colored_text_shader().use_shader(mvp, get_color());
    glActiveTexture(GL_TEXTURE0);
    for (std::size_t char_i = 0; char_i < chars.size(); char_i++) {
        glBindTexture(GL_TEXTURE_2D, chars[char_i].get().texture);
        mesh->bind_vao(true, false, false);
        glDrawArrays(GL_TRIANGLES, char_i * 6, 6);
        mesh->unbind_vao();
    }
}

[[nodiscard]] glm::vec2 TextNode::get_origin() const {
    glm::vec2 result;

    switch (origin_x) {
    case OriginX::LEFT:
        result.x = min_x;
        break;
    case OriginX::CENTER:
        result.x = std::lerp(min_x, max_x, 0.5f);
        break;
    case OriginX::RIGHT:
        result.x = max_x;
        break;
    }

    switch (origin_y) {
    case OriginY::BOTTOM:
        result.y = min_y;
        break;
    case OriginY::CENTER:
        result.y = std::lerp(min_y, max_y, 0.5f);
        break;
    case OriginY::TOP:
        result.y = max_y;
        break;
    }

    return result;
}