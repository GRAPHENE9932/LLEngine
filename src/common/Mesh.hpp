#pragma once

#include <vector>
#include <string>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <GL/glew.h>

class Mesh {
public:
    std::vector<uint16_t> indices;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;

    GLuint indices_id, vertices_id, uvs_id, normals_id;

    explicit Mesh(std::string mesh_path);
    ~Mesh();
};
