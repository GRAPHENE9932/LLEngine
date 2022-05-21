#include "../utils/utils.hpp"

#include "Mesh.hpp"

Mesh::Mesh(std::string mesh_path) {
    utils::load_wavefront_obj(mesh_path, vertices, uvs, normals);

    glGenBuffers(1, &vertices_id);
    glBindBuffer(GL_ARRAY_BUFFER, vertices_id);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &uvs_id);
    glBindBuffer(GL_ARRAY_BUFFER, uvs_id);
    glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), uvs.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &normals_id);
    glBindBuffer(GL_ARRAY_BUFFER, normals_id);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), normals.data(), GL_STATIC_DRAW);
}

Mesh::~Mesh() {
    glDeleteBuffers(1, &vertices_id);
    glDeleteBuffers(1, &uvs_id);
    glDeleteBuffers(1, &normals_id);
}
