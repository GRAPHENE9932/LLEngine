#pragma once

#include <vector>
#include <string>
#include <type_traits> // std::is_same
#include <string_view> // std::string_view

#include <glm/vec2.hpp> // glm::vec2
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <GL/glew.h>

#include "IMesh.hpp" // IMesh

/// Only uint16_t and uint32_t INDEX_T are supported.
template<typename INDEX_T>
class Mesh : public IMesh {
    static_assert(std::is_same<INDEX_T, uint16_t>() || std::is_same<INDEX_T, uint32_t>());
public:
    GLuint get_indices_id() override {return indices_id;}
    GLuint get_vertices_id() override {return vertices_id;}
    GLuint get_uvs_id() override {return uvs_id;}
    GLuint get_normals_id() override {return normals_id;}
    GLuint get_tangents_id() override {return tangents_id;}

    size_t get_amount_of_vertices() override;
    GLenum get_indices_type() override;

    void set_indices(std::vector<INDEX_T> new_indices);
    void set_vertices(std::vector<glm::vec3> new_vertices);
    void set_uvs(std::vector<glm::vec2> new_uvs);
    void set_normals(std::vector<glm::vec3> new_normals);
    void set_tangents(std::vector<glm::vec4> new_tangents);

    bool is_indexed() override;

    Mesh() = default;
    ~Mesh() override;

private:
    GLuint indices_id = 0, vertices_id = 0, uvs_id = 0,
           normals_id = 0, tangents_id = 0;
    std::vector<INDEX_T> indices;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec4> tangents;

    void index_data();
};
