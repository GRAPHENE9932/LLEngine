#include <glm/gtx/transform.hpp>

#include "utils/math.hpp"
#include "utils/shader_loader.hpp"
#include "BitmapTextNode.hpp"
#include "RenderingServer.hpp" // RenderingServer

BitmapTextNode::BitmapTextNode(
    RenderingServer& rs,
    const std::shared_ptr<BitmapFont>& font,
    std::string_view text, const glm::vec3& color,
    const Transform& transform
) : DrawableNode(rs, transform), color(color) {
    set_font(font);
    set_text(text);
}

BitmapTextNode::~BitmapTextNode() {
    // Delete the buffers if they exist.
    if (vertices_id != 0)
        glDeleteBuffers(1, &vertices_id);
    if (uvs_id != 0)
        glDeleteBuffers(1, &uvs_id);
}

void BitmapTextNode::set_font(const std::shared_ptr<BitmapFont>& font) {
    this->font = font;
    this->texture_id = font->get_texture_id();
}

void BitmapTextNode::set_text(std::string_view text) {
    const glm::vec2 char_size = font->get_char_size();
    const glm::vec2 spacing = font->get_spacing();

    vertices.resize(text.size() * QUAD_VERTICES.size());
    uvs.resize(text.size() * QUAD_UVS.size());

    long current_line {0};
    long chars_in_cur_line {0};
    for (std::size_t i = 0; i < text.size(); i++) {
        // Create spans for vertices and UV coordinates.
        std::span<float, QUAD_VERTICES.size()> vertices_span(
                vertices.begin() + i * QUAD_VERTICES.size(), QUAD_VERTICES.size());
        std::span<float, QUAD_UVS.size()> uvs_span(
                uvs.begin() + i * QUAD_UVS.size(), QUAD_UVS.size());
        
        // Compute vertices coordinates.
        if (text[i] == '\n') {
            current_line++;
            chars_in_cur_line = 0;
            continue;
        }
        const glm::vec2 char_pos {
            chars_in_cur_line * (char_size.x + spacing.x),
            -current_line * (char_size.y + spacing.y)
        };
        std::copy(QUAD_VERTICES.begin(), QUAD_VERTICES.end(), vertices_span.begin());
        for (std::size_t j = 0; j < QUAD_VERTICES.size(); j += 3) {
            vertices_span[j] *= char_size.x;
            vertices_span[j + 1] *= char_size.y;

            vertices_span[j] += char_pos.x;
            vertices_span[j + 1] += char_pos.y;
        }

        // Compute UV coordinates.
        font->get_uvs_for_char(text[i], uvs_span);

        chars_in_cur_line++;
    }

    register_buffers();
}

void BitmapTextNode::set_screen_space_position(const glm::vec3& scr_space_pos,
                                               const glm::vec2& win_size) {
    set_translation(glm::vec3(
        utils::scr_space_pos_to_gl_space(scr_space_pos, win_size),
        scr_space_pos.z
    ));
}

void BitmapTextNode::set_screen_space_scale(const glm::vec3& scr_space_scale,
                                            const glm::vec2& win_size) {
    set_scale(glm::vec3(
        utils::scr_space_scale_to_gl_space(
            static_cast<glm::vec2>(scr_space_scale),
            win_size
        ),
        scr_space_scale.z
    ));
}

void BitmapTextNode::update() {
    update_children();

    glEnable(GL_BLEND);

    // Uniforms.
    glm::mat4 model_matrix = get_global_matrix();
    glm::mat4 mvp = rs.get_view_proj_matrix() * model_matrix;
    rs.get_shader_holder().get_colored_text_shader().use_shader(mvp, color);

    // Vertices.
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertices_id);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // UVs.
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, uvs_id);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

    // Bind the texture.
    glBindTexture(GL_TEXTURE_2D, texture_id);

    // Draw.
    glBindBuffer(GL_ARRAY_BUFFER, vertices_id);
    glDrawArrays(GL_TRIANGLES, 0, vertices.size());
    rs.report_about_drawn_triangles(vertices.size() / 3);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    glDisable(GL_BLEND);
}

GLuint BitmapTextNode::get_program_id() const {
    return rs.get_shader_holder().get_colored_text_shader().get_program_id();
}

void BitmapTextNode::register_buffers() {
    // Delete the existing buffers if they exist.
    if (vertices_id != 0)
        glDeleteBuffers(1, &vertices_id);
    if (uvs_id != 0)
        glDeleteBuffers(1, &uvs_id);
    
    // Generate the vertex buffer.
    glGenBuffers(1, &vertices_id);
    glBindBuffer(GL_ARRAY_BUFFER, vertices_id);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // Generate the UV coordinates buffer.
    glGenBuffers(1, &uvs_id);
    glBindBuffer(GL_ARRAY_BUFFER, uvs_id);
    glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(float), uvs.data(), GL_STATIC_DRAW);
}
