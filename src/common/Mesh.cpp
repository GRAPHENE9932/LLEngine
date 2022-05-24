#include "../utils/utils.hpp"

#include "Mesh.hpp"

Mesh::Mesh(std::string mesh_path) {
    std::vector<glm::vec3> unindexed_vertices;
    std::vector<glm::vec2> unindexed_uvs;
    std::vector<glm::vec3> unindexed_normals;

    utils::load_wavefront_obj(mesh_path, unindexed_vertices, unindexed_uvs, unindexed_normals);
    utils::index_vbo(unindexed_vertices, unindexed_uvs, unindexed_normals,
                     indices, vertices, uvs, normals);

    glGenBuffers(1, &indices_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint16_t), indices.data(), GL_STATIC_DRAW);

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
    glDeleteBuffers(1, &indices_id);
    glDeleteBuffers(1, &vertices_id);
    glDeleteBuffers(1, &uvs_id);
    glDeleteBuffers(1, &normals_id);
}
