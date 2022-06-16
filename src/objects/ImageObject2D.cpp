#include <array>

#include <glm/gtx/transform.hpp>

#include "../consts_and_enums.hpp"
#include "../utils/utils.hpp"
#include "ImageObject2D.hpp"

GLuint ImageObject2D::program_id;
GLuint ImageObject2D::vertices_id;
GLuint ImageObject2D::uvs_id;
GLuint ImageObject2D::uvs_inv_v_id;
GLuint ImageObject2D::matrix_uniform_id;

ImageObject2D::ImageObject2D(std::shared_ptr<Texture> texture, const Rect& rect,
    bool is_transparent) :
    is_transparent(is_transparent) {
    change_rect(rect);
    change_texture(texture);
}

void ImageObject2D::change_rect(const Rect& new_rect) {
    matrix = glm::translate(glm::vec3(new_rect.position.x, new_rect.position.y, 0.0f)) *
        glm::scale(glm::vec3(new_rect.size.x, new_rect.size.y, 0.0f));
}

void ImageObject2D::change_texture(std::shared_ptr<Texture> texture) {
    this->texture = texture;
}

void ImageObject2D::pre_init() {
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
    program_id = utils::load_shaders("res/shaders/2d_vertex.glsl", "res/shaders/2d_fragment.glsl");
    matrix_uniform_id = glGetUniformLocation(program_id, "OBJECT_MATRIX");
}

void ImageObject2D::clean_up() {
    glDeleteBuffers(1, &vertices_id);
    glDeleteBuffers(1, &uvs_id);

    glDeleteProgram(program_id);
}

void ImageObject2D::draw() const {
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
    glUniformMatrix4fv(matrix_uniform_id, 1, GL_FALSE, &matrix[0][0]);

    glBindBuffer(GL_ARRAY_BUFFER, vertices_id);
    glDrawArrays(GL_TRIANGLES, 0, QUAD_VERTICES.size());

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    if (is_transparent)
        glDisable(GL_BLEND);
}
