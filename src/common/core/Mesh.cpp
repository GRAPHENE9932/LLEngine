#include <unordered_map>
#include <utility>
#include <fstream>
#include <sstream>
#include <array>

#include "Mesh.hpp"

template<typename INDEX_T>
size_t Mesh<INDEX_T>::get_amount_of_vertices() {
    if (is_indexed())
        return indices.size();
    else
        return vertices.size();
}

template<typename INDEX_T>
GLuint Mesh<INDEX_T>::get_indices_type() {
    return 0;
}

template<>
GLenum Mesh<uint16_t>::get_indices_type() {
    return GL_UNSIGNED_SHORT;
}

template<>
GLenum Mesh<uint32_t>::get_indices_type() {
    return GL_UNSIGNED_INT;
}

template<typename T, GLenum TARGET>
void handle_buffer(std::vector<T>& buffer, GLuint& buffer_id) {
    if (!buffer_id)
        glGenBuffers(1, &buffer_id);

    glBindBuffer(TARGET, buffer_id);
    glBufferData(TARGET, buffer.size() * sizeof(T),
                 buffer.data(), GL_STATIC_DRAW);
}

template<typename INDEX_T>
void Mesh<INDEX_T>::set_indices(const std::vector<INDEX_T>& new_indices) {
    indices = new_indices;
    handle_buffer<INDEX_T, GL_ELEMENT_ARRAY_BUFFER>(indices, indices_id);
}

template<typename INDEX_T>
void Mesh<INDEX_T>::set_vertices(const std::vector<glm::vec3>& new_vertices) {
    vertices = new_vertices;
    handle_buffer<glm::vec3, GL_ARRAY_BUFFER>(vertices, vertices_id);
}

template<typename INDEX_T>
void Mesh<INDEX_T>::set_uvs(const std::vector<glm::vec2>& new_uvs) {
    uvs = new_uvs;
    handle_buffer<glm::vec2, GL_ARRAY_BUFFER>(uvs, uvs_id);
}

template<typename INDEX_T>
void Mesh<INDEX_T>::set_normals(const std::vector<glm::vec3>& new_normals) {
    normals = new_normals;
    handle_buffer<glm::vec3, GL_ARRAY_BUFFER>(normals, normals_id);
}

template<typename INDEX_T>
void Mesh<INDEX_T>::set_tangents(const std::vector<glm::vec4>& new_tangents) {
    tangents = new_tangents;
    handle_buffer<glm::vec4, GL_ARRAY_BUFFER>(tangents, tangents_id);
}

template<typename INDEX_T>
bool Mesh<INDEX_T>::is_indexed() {
    return static_cast<bool>(get_indices_id());
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

template<typename INDEX_T>
void Mesh<INDEX_T>::index_data() {
    if (vertices.size() != uvs.size() ||
        vertices.size() != normals.size()) {
        throw std::runtime_error("Can't index mesh data: unequal amounts of components.");
    }
    
    // Copy input data.
    auto in_vertices = vertices;
    auto in_uvs = uvs;
    auto in_normals = normals;

    indices.clear();
    vertices.clear();
    uvs.clear();
    normals.clear();

    std::unordered_map<CompleteVertex, INDEX_T, CompleteVertexHasher> vertex_to_index;

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

template<typename INDEX_T>
Mesh<INDEX_T>::~Mesh() {
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

template class Mesh<uint16_t>;
template class Mesh<uint32_t>;
