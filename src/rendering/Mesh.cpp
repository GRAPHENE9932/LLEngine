#include <unordered_map>
#include <utility>
#include <fstream>
#include <sstream>
#include <array>

#include "rendering/Mesh.hpp"

using namespace llengine;

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

void Mesh::bind_vao(bool enable_uv, bool enable_normals, bool enable_tangents) const {
    if (vao_id == 0) {
        initialize_vao();
    }

    glBindVertexArray(vao_id);
    if (vertices_id != 0) glEnableVertexAttribArray(0);
    if (uvs_id != 0 && enable_uv) glEnableVertexAttribArray(1);
    if (normals_id != 0 && enable_normals) glEnableVertexAttribArray(2);
    if (tangents_id != 0 && enable_tangents) glEnableVertexAttribArray(3);
}

void Mesh::unbind_vao() const {
    if (vao_id != 0) {
        glBindVertexArray(0);
        if (vertices_id != 0) glDisableVertexAttribArray(0);
        if (uvs_id != 0) glDisableVertexAttribArray(1);
        if (normals_id != 0) glDisableVertexAttribArray(2);
        if (tangents_id != 0) glDisableVertexAttribArray(3);
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
    reset_vao_if_needed();
}

void Mesh::set_uvs(const std::vector<glm::vec2>& new_uvs) {
    uvs = new_uvs;
    handle_buffer<glm::vec2, GL_ARRAY_BUFFER>(uvs, uvs_id);
    reset_vao_if_needed();
}

void Mesh::set_normals(const std::vector<glm::vec3>& new_normals) {
    normals = new_normals;
    handle_buffer<glm::vec3, GL_ARRAY_BUFFER>(normals, normals_id);
    reset_vao_if_needed();
}

void Mesh::set_tangents(const std::vector<glm::vec4>& new_tangents) {
    tangents = new_tangents;
    handle_buffer<glm::vec4, GL_ARRAY_BUFFER>(tangents, tangents_id);
    reset_vao_if_needed();
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

Mesh::Mesh(const Mesh& other) {
    std::visit([this] (const auto& vector) {
        this->set_indices(vector);
    }, other.indices);
    set_vertices(other.vertices);
    set_uvs(other.uvs);
    set_normals(other.normals);
    set_tangents(other.tangents);
}

Mesh::Mesh(Mesh&& other) noexcept :
    indices(std::move(other.indices)),
    vertices(std::move(other.vertices)),
    uvs(std::move(other.uvs)),
    normals(std::move(other.normals)),
    tangents(std::move(other.tangents)),
    vao_id(other.vao_id),
    indices_id(other.indices_id),
    vertices_id(other.vertices_id),
    uvs_id(other.uvs_id),
    normals_id(other.normals_id),
    tangents_id(other.tangents_id) {}

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
    if (vao_id)
        glDeleteVertexArrays(1, &vao_id);
}

Mesh& Mesh::operator=(const Mesh& other) {
    std::visit([this] (const auto& vector) {
        this->set_indices(vector);
    }, other.indices);
    set_vertices(other.vertices);
    set_uvs(other.uvs);
    set_normals(other.normals);
    set_tangents(other.tangents);

    return *this;
}

Mesh& Mesh::operator=(Mesh&& other) noexcept {
    indices = std::move(other.indices);
    vertices = std::move(other.vertices);
    uvs = std::move(other.uvs);
    normals = std::move(other.normals);
    tangents = std::move(other.tangents);

    indices_id = other.indices_id;
    vertices_id = other.vertices_id;
    uvs_id = other.uvs_id;
    normals_id = other.normals_id;
    tangents_id = other.tangents_id;

    return *this;
}

void bind_vertex_attrib_pointer(GLuint buffer_id, GLuint vertex_attrib_index, GLint size) {
    if (buffer_id != 0) {
        glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
        glVertexAttribPointer(vertex_attrib_index, size, GL_FLOAT, GL_FALSE, 0, 0);
    }
}

void Mesh::reset_vao_if_needed() const {
    if (vao_id != 0) {
        glDeleteVertexArrays(1, &vao_id);
        initialize_vao();
    }
}

void Mesh::initialize_vao() const {
    glGenVertexArrays(1, &vao_id);
    glBindVertexArray(vao_id);

    bind_vertex_attrib_pointer(vertices_id, 0, 3);
    bind_vertex_attrib_pointer(uvs_id,      1, 2);
    bind_vertex_attrib_pointer(normals_id,  2, 3);
    bind_vertex_attrib_pointer(tangents_id, 3, 4);
}

template void Mesh::set_indices(const std::vector<uint16_t> &new_indices);
template void Mesh::set_indices(const std::vector<uint32_t> &new_indices);