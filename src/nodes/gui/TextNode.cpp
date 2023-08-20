#include "nodes/gui/TextNode.hpp"
#include "node_registration.hpp"
#include "rendering/RenderingServer.hpp"
#include "utils/math.hpp"

#include <glm/gtx/transform.hpp>
#include <glm/mat4x4.hpp>
#include <GL/glew.h>

using namespace llengine;

constexpr std::string_view VERTEX_SHADER_TEXT =
    #include "shaders/colored_text.vert"
;
constexpr std::string_view FRAGMENT_SHADER_TEXT =
    #include "shaders/colored_text.frag"
;

[[nodiscard]] static std::vector<std::string_view> split_in_lines(
    std::string_view text, const FreeTypeFont& font, float maximum_width
) {
    std::vector<std::string_view> result;
    float cur_width = 0.0f;
    std::string_view::const_iterator cur_line_start = text.begin();

    for (std::size_t i = 0; i < text.size(); i++) {
        if (text[i] == '\n') {
            result.emplace_back(cur_line_start, text.begin() + i);
            cur_line_start = text.begin() + i + 1;
            cur_width = 0.0f;
            continue;
        }

        if (maximum_width != 0.0f && cur_width >= maximum_width) {
            result.emplace_back(cur_line_start, text.begin() + i);
            cur_line_start = text.begin() + i;
            cur_width = 0.0f;
            continue;
        }

        cur_width += font.get_font_char(text[i]).advance / 64.0f;
    }

    result.emplace_back(cur_line_start, text.end());
    return result;
}

[[nodiscard]] GUITransform TextNode::get_transform() const {
    return {
        position_anchor,
        position_offset,
        z_coordinate,
        origin_x,
        origin_y,
        GUITransform::SizeMode::ABSOLUTE,
        get_absolute_size()
    };
}

[[nodiscard]] glm::vec2 TextNode::get_absolute_size() const {
    return {
        max_x - min_x,
        max_y - min_y
    };
}

void TextNode::set_transform(const GUITransform& transform) {
    this->position_anchor = transform.position_anchor;
    this->position_offset = transform.position_offset;
    this->z_coordinate = transform.z_coordinate;
    this->origin_x = transform.origin_x;
    this->origin_y = transform.origin_y;
    // We can't modify the size, because it depends on the text and font.
}

void TextNode::set_text(std::string_view new_text) {
    cached_text = new_text;

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

    auto lines = split_in_lines(new_text, *font, max_width);
    float offset_y = 0.0f;
    for (std::string_view line : lines) {
        add_line({0.0f, offset_y}, line, vertices, uvs);
        offset_y -= font->get_font_size();
    }

    mesh = std::make_optional<Mesh>();
    mesh->set_vertices(vertices);
    mesh->set_uvs(uvs);
}

void TextNode::set_text_property(const NodeProperty& property) {
    set_text(property.get<std::string>());
}

void TextNode::set_color_property(const NodeProperty& property) {
    set_color(property.get<glm::vec3>());
}

void TextNode::set_font(const std::shared_ptr<FreeTypeFont>& new_font) {
    font = new_font;
    set_text(cached_text);
}

void TextNode::set_font_property(const NodeProperty& property) {
    set_font(FreeTypeFont::from_property(property));
}

void TextNode::set_max_width(float max_width) {
    this->max_width = max_width;
    set_text(cached_text);
}

void TextNode::set_max_width_property(const NodeProperty& property) {
    set_max_width(property.get<float>());
}

void TextNode::draw() {
    draw_children();

    const glm::vec2 window_size {get_rendering_server().get_window().get_window_size()};
    glm::vec3 absolute_position_in_pixels {get_screen_space_position()};
    absolute_position_in_pixels.y += max_y - min_y;
    const glm::vec3 absolute_position_opengl {math_utils::scr_space_pos_to_gl_space(absolute_position_in_pixels, window_size)};

    const glm::vec3 pixels_to_opengl_scale {2.0f / window_size, 1.0f};
    const glm::mat4 mvp {glm::translate(absolute_position_opengl) * glm::scale(pixels_to_opengl_scale)};

    ensure_shader_is_initialized();
    shader->use_shader();
    shader->set_mat4<"mvp">(mvp);
    shader->set_vec3<"text_color">(get_color());
    glActiveTexture(GL_TEXTURE0);
    for (std::size_t char_i = 0; char_i < chars.size(); char_i++) {
        glBindTexture(GL_TEXTURE_2D, chars[char_i].get().texture);
        mesh->bind_vao(true, false, false);
        glDrawArrays(GL_TRIANGLES, char_i * 6, 6);
        mesh->unbind_vao();
    }
}

void TextNode::register_properties() {
    register_custom_property<TextNode>("text_node", "text_color", &TextNode::set_color_property);
    register_custom_property<TextNode>("text_node", "text", &TextNode::set_text_property);
    register_custom_property<TextNode>("text_node", "font", &TextNode::set_font_property);
    register_custom_property<TextNode>("text_node", "max_width", &TextNode::set_max_width_property);
}

void TextNode::add_line(
    glm::vec2 offset, std::string_view line,
    std::vector<glm::vec3>& vertices, std::vector<glm::vec2>& uvs
) {
    float baseline_x {0.0f};
    for (char c : line) {
        const FreeTypeFont::FontChar& font_char = font->get_font_char(c);
        chars.push_back(std::ref(font_char));

        const float left_x = baseline_x + font_char.position.x + offset.x;
        const float right_x = left_x + font_char.texture.get_size().x + offset.x;
        const float top_y = static_cast<float>(font_char.position.y) + offset.y;
        const float bottom_y = static_cast<float>(font_char.position.y) - static_cast<float>(font_char.texture.get_size().y) + offset.y;

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
}

void TextNode::ensure_shader_is_initialized() {
    if (shader) {
        return;
    }

    shader = std::make_unique<ShaderType>(VERTEX_SHADER_TEXT, FRAGMENT_SHADER_TEXT);
}