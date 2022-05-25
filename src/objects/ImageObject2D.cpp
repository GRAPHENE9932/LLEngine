#include <array>

#include <glm/gtx/transform.hpp>

#include "../utils/utils.hpp"
#include "ImageObject2D.hpp"

constexpr std::array<GLfloat, 12> square_vertices {
    -1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    -1.0f, -1.0f, 0.0f
};

constexpr std::array<GLfloat, 8> square_uvs {
    0.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f
};

constexpr std::array<GLshort, 6> square_indices {
    3, 1, 0,
    3, 2, 1
};

GLuint ImageObject2D::program_id;
GLuint ImageObject2D::indices_id;
GLuint ImageObject2D::vertices_id;
GLuint ImageObject2D::uvs_id;
GLuint ImageObject2D::matrix_uniform_id;

ImageObject2D::ImageObject2D(GLuint texture_id, const Rect& rect) {
    change_rect(rect);
    change_texture(texture_id);
}

void ImageObject2D::change_rect(const Rect& new_rect) {
    matrix = glm::translate(glm::vec3(new_rect.position.x, new_rect.position.y, 0.0f)) *
        glm::scale(glm::vec3(new_rect.size.x, new_rect.size.y, 0.0f));
}

void ImageObject2D::change_texture(GLuint texture_id) {
    this->texture_id = texture_id;
}

void ImageObject2D::pre_init() {
    // Init buffers.
    glGenBuffers(1, &indices_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, square_indices.size() * sizeof(uint16_t),
                 square_indices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &vertices_id);
    glBindBuffer(GL_ARRAY_BUFFER, vertices_id);
    glBufferData(GL_ARRAY_BUFFER, square_vertices.size() * sizeof(glm::vec3),
                 square_vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &uvs_id);
    glBindBuffer(GL_ARRAY_BUFFER, uvs_id);
    glBufferData(GL_ARRAY_BUFFER, square_uvs.size() * sizeof(glm::vec2),
                 square_uvs.data(), GL_STATIC_DRAW);

    // Init shaders.
    program_id = utils::load_shaders("res/shaders/2d_vertex.glsl", "res/shaders/2d_fragment.glsl");
    matrix_uniform_id = glGetUniformLocation(program_id, "OBJECT_MATRIX");
}

void ImageObject2D::clean_up() {
    glDeleteBuffers(1, &indices_id);
    glDeleteBuffers(1, &vertices_id);
    glDeleteBuffers(1, &uvs_id);

    glDeleteProgram(program_id);
}

void ImageObject2D::draw() const {
    glEnable(GL_BLEND);

    // Vertices.
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertices_id);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // UVs.
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, uvs_id);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

    // Bind textures.
    glBindTexture(GL_TEXTURE_2D, texture_id);

    // Uniforms.
    glUniformMatrix4fv(matrix_uniform_id, 1, GL_FALSE, &matrix[0][0]);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);
    glDrawElements(GL_TRIANGLES, square_indices.size(), GL_UNSIGNED_SHORT, 0);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    glDisable(GL_BLEND);
}
