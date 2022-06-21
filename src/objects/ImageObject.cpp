#include <array>

#include <glm/gtx/transform.hpp>

#include "../consts_and_enums.hpp"
#include "../utils/utils.hpp"
#include "../utils/math.hpp"
#include "ImageObject.hpp"

GLuint ImageObject::program_id;
GLuint ImageObject::vertices_id;
GLuint ImageObject::uvs_id;
GLuint ImageObject::uvs_inv_v_id;
GLuint ImageObject::model_matrix_uniform_id;
GLuint ImageObject::mvp_matrix_uniform_id;

ImageObject::ImageObject(std::shared_ptr<Texture> texture, bool is_transparent, bool is_2d) :
    is_transparent(is_transparent), is_2d(is_2d) {
    change_texture(texture);
}

void ImageObject::change_texture(std::shared_ptr<Texture> texture) {
    this->texture = texture;
}

void ImageObject::pre_init() {
    // Init buffers.
    glGenBuffers(1, &vertices_id);
    glBindBuffer(GL_ARRAY_BUFFER, vertices_id);
    glBufferData(GL_ARRAY_BUFFER, QUAD_VERTICES.size() * sizeof(glm::vec3),
                 QUAD_VERTICES.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &uvs_id);
    glBindBuffer(GL_ARRAY_BUFFER, uvs_id);
    glBufferData(GL_ARRAY_BUFFER, QUAD_UVS.size() * sizeof(glm::vec2),
                 QUAD_UVS.data(), GL_STATIC_DRAW);
                 
    glGenBuffers(1, &uvs_inv_v_id);
    glBindBuffer(GL_ARRAY_BUFFER, uvs_inv_v_id);
    glBufferData(GL_ARRAY_BUFFER, QUAD_UVS_INV_V.size() * sizeof(glm::vec2),
                 QUAD_UVS_INV_V.data(), GL_STATIC_DRAW);

    // Init shaders.
    program_id = utils::load_shaders("res/shaders/unshaded_textured_vertex.glsl",
            "res/shaders/unshaded_textured_fragment.glsl");
    model_matrix_uniform_id = glGetUniformLocation(program_id, "MODEL_MATRIX");
    mvp_matrix_uniform_id = glGetUniformLocation(program_id, "MVP");
}

void ImageObject::clean_up() {
    glDeleteBuffers(1, &vertices_id);
    glDeleteBuffers(1, &uvs_id);

    glDeleteProgram(program_id);
}

void ImageObject::set_screen_space_position(const glm::vec3& scr_space_pos, const glm::vec2 win_size) {
    translation = glm::vec3(
        utils::scr_space_pos_to_gl_space(scr_space_pos, win_size),
        scr_space_pos.z
    );
}

void ImageObject::set_in_center_of_screen(const glm::vec2 win_size, const float z_coord) {
    translation = glm::vec3(
        -static_cast<glm::vec2>(texture->get_size()) / win_size,
        z_coord
    );
}

void ImageObject::set_screen_space_scale(const glm::vec3& scr_space_scale, const glm::vec2 win_size) {
    scale = glm::vec3(
        utils::scr_space_scale_to_gl_space(
            static_cast<glm::vec2>(scr_space_scale) * static_cast<glm::vec2>(texture->get_size()),
            win_size
        ),
        scr_space_scale.z
    );
}

void ImageObject::draw(DrawParameters& params) {
    if (program_id == 0)
        pre_init();
    
    if (params.cur_shader != program_id) {
        params.cur_shader = program_id;
        glUseProgram(program_id);
    }

    if (is_transparent)
        glEnable(GL_BLEND);

    // Vertices.
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertices_id);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // UVs.
    glEnableVertexAttribArray(1);
    if (texture->get_is_compressed())
        glBindBuffer(GL_ARRAY_BUFFER, uvs_inv_v_id);
    else
        glBindBuffer(GL_ARRAY_BUFFER, uvs_id);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

    // Bind textures.
    glBindTexture(GL_TEXTURE_2D, texture->get_id());

    // Uniforms.
    glm::mat4 model_matrix = compute_matrix();
    glm::mat4 mvp = is_2d ? model_matrix : params.view_proj_matrix * model_matrix;

    glUniformMatrix4fv(mvp_matrix_uniform_id, 1, GL_FALSE, &mvp[0][0]);

    glBindBuffer(GL_ARRAY_BUFFER, vertices_id);
    glDrawArrays(GL_TRIANGLES, 0, QUAD_VERTICES.size());

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    if (is_transparent)
        glDisable(GL_BLEND);
}
