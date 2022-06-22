#pragma once

#include <vector>
#include <string>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <GL/glew.h>

class Mesh {
public:
    inline GLuint get_indices_id() {
        return indices_id;
    }
    inline GLuint get_vertices_id() {
        return vertices_id;
    }
    inline GLuint get_uvs_id() {
        return uvs_id;
    }
    inline GLuint get_normals_id() {
        return normals_id;
    }

    inline const std::vector<uint16_t>& get_indices() {
        return indices;
    }
    inline const std::vector<glm::vec3>& get_vertices() {
        return vertices;
    }
    inline const std::vector<glm::vec2>& get_uvs() {
        return uvs;
    }
    inline const std::vector<glm::vec3>& get_normals() {
        return normals;
    }

    explicit Mesh(std::string mesh_path);
    ~Mesh();

private:
    GLuint indices_id, vertices_id, uvs_id, normals_id;

    std::vector<uint16_t> indices;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
};
