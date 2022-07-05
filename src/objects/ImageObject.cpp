#include <array>

#include <glm/gtx/transform.hpp>

#include "../utils/shader_loader.hpp"
#include "../consts_and_enums.hpp"
#include "../utils/math.hpp"
#include "ImageObject.hpp"

GLuint ImageObject::vertices_id = 0;
GLuint ImageObject::uvs_id = 0;

ImageObject::ImageObject(std::shared_ptr<Texture> texture, bool is_transparent, bool is_2d) :
    is_transparent(is_transparent), is_2d(is_2d) {
    change_texture(texture);
}

void ImageObject::change_texture(std::shared_ptr<Texture> texture) {
    this->texture = texture;
}

void ImageObject::static_init() {
    // Init buffers.
    glGenBuffers(1, &vertices_id);
    glBindBuffer(GL_ARRAY_BUFFER, vertices_id);
    glBufferData(GL_ARRAY_BUFFER, QUAD_VERTICES.size() * sizeof(float),
                 QUAD_VERTICES.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &uvs_id);
    glBindBuffer(GL_ARRAY_BUFFER, uvs_id);
    glBufferData(GL_ARRAY_BUFFER, QUAD_UVS.size() * sizeof(float),
                 QUAD_UVS.data(), GL_STATIC_DRAW);
}

void ImageObject::static_clean_up() {
    vertices_id = 0;
    uvs_id = 0;

    glDeleteBuffers(1, &vertices_id);
    glDeleteBuffers(1, &uvs_id);
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
    if (is_transparent)
        glEnable(GL_BLEND);

    // Uniforms.
    glm::mat4 model_matrix {compute_matrix()};
    glm::mat4 mvp {is_2d ? model_matrix : params.view_proj_matrix * model_matrix};
    params.sh_mgr.use_unshaded_textured_shader(mvp);

    // Vertices.
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertices_id);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // UVs.
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, uvs_id);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

    // Bind textures.
    glBindTexture(GL_TEXTURE_2D, texture->get_id());

    glBindBuffer(GL_ARRAY_BUFFER, vertices_id);
    glDrawArrays(GL_TRIANGLES, 0, QUAD_VERTICES.size());
    params.triangles_drawn += QUAD_VERTICES.size() / 3;

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    if (is_transparent)
        glDisable(GL_BLEND);
}

GLuint ImageObject::get_program_id(DrawParameters& params) const {
    return params.sh_mgr.get_unshaded_textured_program_id();
}
