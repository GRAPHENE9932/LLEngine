#include <unordered_map>
#include <utility>
#include <fstream>
#include <sstream>
#include <array>

#include "Mesh.hpp"

size_t Mesh::get_amount_of_vertices() const {
    if (is_indexed()) {
        return std::visit([] (const auto& vector) -> std::size_t {
            return vector.size();
        }, indices);
    }
    else {
        return vertices.size();
    }
}

GLenum Mesh::get_indices_type() const {
    if (std::holds_alternative<std::vector<uint16_t>>(indices)) {
        return GL_UNSIGNED_SHORT;
    }
    else {
        return GL_UNSIGNED_INT;
    }
}

template<typename T, GLenum TARGET>
void handle_buffer(std::vector<T>& buffer, GLuint& buffer_id) {
    if (!buffer_id)
        glGenBuffers(1, &buffer_id);

    glBindBuffer(TARGET, buffer_id);
    glBufferData(TARGET, buffer.size() * sizeof(T),
                 buffer.data(), GL_STATIC_DRAW);
}

template<typename T>
void Mesh::set_indices(const std::vector<T>& new_indices) {
    indices = new_indices;
    handle_buffer<T, GL_ELEMENT_ARRAY_BUFFER>(
        std::get<std::vector<T>>(indices),
        indices_id
    );
}

void Mesh::set_vertices(const std::vector<glm::vec3>& new_vertices) {
    vertices = new_vertices;
    handle_buffer<glm::vec3, GL_ARRAY_BUFFER>(vertices, vertices_id);
}

void Mesh::set_uvs(const std::vector<glm::vec2>& new_uvs) {
    uvs = new_uvs;
    handle_buffer<glm::vec2, GL_ARRAY_BUFFER>(uvs, uvs_id);
}

void Mesh::set_normals(const std::vector<glm::vec3>& new_normals) {
    normals = new_normals;
    handle_buffer<glm::vec3, GL_ARRAY_BUFFER>(normals, normals_id);
}

void Mesh::set_tangents(const std::vector<glm::vec4>& new_tangents) {
    tangents = new_tangents;
    handle_buffer<glm::vec4, GL_ARRAY_BUFFER>(tangents, tangents_id);
}

struct CompleteVertex {
    glm::vec3 vertex;
    glm::vec2 uv;
    glm::vec3 normal;

    bool operator==(const CompleteVertex& other) const noexcept {
        return vertex == other.vertex && uv == other.uv && normal == other.normal;
    }
};

struct CompleteVertexHasher {
    std::size_t operator()(const CompleteVertex& cv) const {
        size_t res = 17;
        std::hash<float> hash;

        res = res * 31 + hash(cv.vertex.x);
        res = res * 31 + hash(cv.vertex.y);
        res = res * 31 + hash(cv.vertex.z);
        res = res * 31 + hash(cv.uv.x);
        res = res * 31 + hash(cv.uv.y);
        res = res * 31 + hash(cv.normal.x);
        res = res * 31 + hash(cv.normal.y);
        res = res * 31 + hash(cv.normal.z);

        return res;
    }
};

template<typename T>
void Mesh::index_data() {
    if (vertices.size() != uvs.size() ||
        vertices.size() != normals.size()) {
        throw std::runtime_error("Can't index mesh data: unequal amounts of components.");
    }

    auto& indices = std::get<std::vector<T>>(this->indices);
    
    // Copy input data.
    auto in_vertices = vertices;
    auto in_uvs = uvs;
    auto in_normals = normals;

    indices.clear();
    vertices.clear();
    uvs.clear();
    normals.clear();

    std::unordered_map<CompleteVertex, T, CompleteVertexHasher> vertex_to_index;

    for (std::size_t i = 0; i < in_vertices.size(); i++) {
        CompleteVertex complete {in_vertices[i], in_uvs[i], in_normals[i]};

        auto find_result = vertex_to_index.find(complete);

        if (find_result != vertex_to_index.end()) {
            // Needed vertex already present in the out_* arguments.
            // Add index only.
            indices.push_back(find_result->second);
        }
        else {
            // Needed vertex doesn't present in the out_* arguments.
            // Add the vertex and it's index.
            vertices.push_back(complete.vertex);
            uvs.push_back(complete.uv);
            normals.push_back(complete.normal);

            uint32_t new_index = vertices.size() - 1;
            vertex_to_index.insert(std::make_pair(complete, new_index));
            indices.push_back(new_index);
        }
    }
}

void Mesh::index_data() {
    if (std::holds_alternative<std::vector<uint16_t>>(indices)) {
        index_data<uint16_t>();
    }
    else {
        index_data<uint32_t>();
    }
}

Mesh::~Mesh() {
    if (indices_id)
        glDeleteBuffers(1, &indices_id);
    if (vertices_id)
        glDeleteBuffers(1, &vertices_id);
    if (uvs_id)
        glDeleteBuffers(1, &uvs_id);
    if (normals_id)
        glDeleteBuffers(1, &normals_id);
    if (tangents_id)
        glDeleteBuffers(1, &tangents_id);
}

template void Mesh::set_indices(const std::vector<uint16_t> &new_indices);
template void Mesh::set_indices(const std::vector<uint32_t> &new_indices);
