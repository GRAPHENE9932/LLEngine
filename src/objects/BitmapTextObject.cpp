#include <glm/gtx/transform.hpp>

#include "../utils/utils.hpp"
#include "../utils/math.hpp"
#include "BitmapTextObject.hpp"

GLuint BitmapTextObject::program_id = 0;
GLuint BitmapTextObject::mvp_matrix_uniform_id = 0;
GLuint BitmapTextObject::color_uniform_id = 0;

BitmapTextObject::BitmapTextObject(const std::shared_ptr<BitmapFont>& font,
        const std::string& text, const glm::vec3& color,
        const glm::vec2 win_size, const bool is_2d) :
        color(color), is_2d(is_2d) {
    set_font(font);
    set_text(text);
}

BitmapTextObject::~BitmapTextObject() {
    // Delete the buffers if they exist.
    if (vertices_id != 0)
        glDeleteBuffers(1, &vertices_id);
    if (uvs_id != 0)
        glDeleteBuffers(1, &uvs_id);
}

void BitmapTextObject::pre_init() {
    // Init shaders.
    program_id = utils::load_shaders("res/shaders/colored_text_vertex.glsl",
            "res/shaders/colored_text_fragment.glsl");
    mvp_matrix_uniform_id = glGetUniformLocation(program_id, "MVP");
    color_uniform_id = glGetUniformLocation(program_id, "COLOR");
}

void BitmapTextObject::clean_up() {
    glDeleteProgram(program_id);
}

void BitmapTextObject::set_screen_space_position(const glm::vec3& scr_space_pos, const glm::vec2 win_size) {
    translation = glm::vec3(
        utils::scr_space_pos_to_gl_space(scr_space_pos, win_size),
        scr_space_pos.z
    );
}

void BitmapTextObject::set_screen_space_scale(const glm::vec3& scr_space_scale, const glm::vec2 win_size) {
    scale = glm::vec3(
        utils::scr_space_scale_to_gl_space(
            static_cast<glm::vec2>(scr_space_scale),
            win_size
        ),
        scr_space_scale.z
    );
}

void BitmapTextObject::set_font(const std::shared_ptr<BitmapFont>& font) {
    this->font = font;
    this->texture_id = font->get_texture_id();
}

void BitmapTextObject::set_text(const std::string& text) {
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

void BitmapTextObject::draw(DrawParameters& params) {
    if (program_id == 0)
        pre_init();
    
    if (params.cur_shader != program_id) {
        params.cur_shader = program_id;
        glUseProgram(program_id);
    }

    glEnable(GL_BLEND);

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

    // Uniforms.
    glm::mat4 model_matrix = compute_matrix();
    glm::mat4 mvp = is_2d ? model_matrix : params.view_proj_matrix * model_matrix;

    glUniformMatrix4fv(mvp_matrix_uniform_id, 1, GL_FALSE, &mvp[0][0]);

    glUniform3fv(color_uniform_id, 1, &color[0]);

    // Draw.
    glBindBuffer(GL_ARRAY_BUFFER, vertices_id);
    glDrawArrays(GL_TRIANGLES, 0, vertices.size());

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    glDisable(GL_BLEND);
}

void BitmapTextObject::register_buffers() {
    // Delete the buffers if they already exist.
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