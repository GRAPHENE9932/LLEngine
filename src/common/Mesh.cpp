#include <unordered_map>
#include <fstream>
#include <sstream>
#include <array>

#include "Mesh.hpp"

/// Load the wavefront OBJ file (limited).
/// Limitations:
///  - List of vertices, UVs and normals must be before the list of faces.
///  - All faces must have vertex, UV and normal index at the same time.
///  - Materials are ignored.
///  - Parameter space vertices are ignored.
///  - Line elements are ignored.
///  - W coordinates in both vertices list and UVs list are ignored.
///  - Objects and groups are ignored.
///  - Smooth shading option is ignored.
void load_wavefront_obj(std::string file_path, std::vector<glm::vec3>& vertices_out,
        std::vector<glm::vec2>& uvs_out, std::vector<glm::vec3>& normals_out) {
    std::ifstream stream(file_path, std::ios::in);
    if (!stream)
        throw std::runtime_error("Failed to read the .obj file. File path: " + file_path);

    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;

    std::string line;
    while (std::getline(stream, line)) {
        std::istringstream ss(line);

        // Process the first token.
        std::string first_token;
        ss >> first_token;

        // It's a comment. Skip.
        if (first_token == "#") {
            continue;
        }
        // I don't support materials, lines, objects, groups and smooth shading. Skip.
        else if (
            first_token == "mtllib" || first_token == "usemtl" || first_token == "l" ||
            first_token == "o" || first_token == "g" || first_token == "s"
        ) {
            continue;
        }
        // The vertex.
        else if (first_token == "v") {
            glm::vec3 cur_vertex;
            ss >> cur_vertex.x;
            ss >> cur_vertex.y;
            ss >> cur_vertex.z;
            if (!ss)
                throw std::runtime_error(
                    "Failed to parse the .obj file:\n"
                    "error on vertex coordinates parsing.\n"
                    "File path: " + file_path
                );
            vertices.push_back(cur_vertex);
        }
        // Texture coordinates (UV).
        else if (first_token == "vt") {
            glm::vec2 cur_uv;
            ss >> cur_uv.x;
            ss >> cur_uv.y;
            if (!ss)
                throw std::runtime_error(
                    "Failed to parse the .obj file:\n"
                    "error on texture coordinates parsing.\n"
                    "File path: " + file_path
                );
            uvs.push_back(cur_uv);
        }
        // Vertex normals.
        else if (first_token == "vn") {
            glm::vec3 cur_normal;
            ss >> cur_normal.x;
            ss >> cur_normal.y;
            ss >> cur_normal.z;
            if (!ss)
                throw std::runtime_error(
                    "Failed to parse the .obj file:\n"
                    "error on vertex normals parsing.\n"
                    "File path: " + file_path
                );
            normals.push_back(cur_normal);
        }
        // Faces.
        else if (first_token == "f") {
            for (int i = 0; i < 3; i++) {
                std::string indices_string;
                ss >> indices_string;
                if (!ss)
                    throw std::runtime_error(
                        "Failed to parse the .obj file:\n"
                        "error on faces parsing.\n"
                        "File path: " + file_path
                    );

                std::array<int, 3> indices;
                std::istringstream indices_ss(indices_string);
                for (int i = 0; i < 3; i++) {
                    std::string index_string;
                    if (!std::getline(indices_ss, index_string, '/'))
                        throw std::runtime_error(
                            "Failed to parse the .obj file:\n"
                            "error on face indices parsing (not enough of them).\n"
                            "File path: " + file_path
                        );
                    indices[i] = std::stoi(index_string);
                }

                vertices_out.push_back(vertices.at(indices[0] - 1));
                uvs_out.push_back(uvs.at(indices[1] - 1));
                normals_out.push_back(normals.at(indices[2] - 1));
            }
        }
    }
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

void index_vbo(std::vector<glm::vec3>& in_vertices,
        std::vector<glm::vec2>& in_uvs,
        std::vector<glm::vec3>& in_normals,
        std::vector<uint16_t>& out_indices,
        std::vector<glm::vec3>& out_vertices,
        std::vector<glm::vec2>& out_uvs,
        std::vector<glm::vec3>& out_normals) {
    assert(in_vertices.size() == in_uvs.size() && in_vertices.size() == in_normals.size());

    out_indices.clear();
    out_vertices.clear();
    out_uvs.clear();
    out_normals.clear();

    std::unordered_map<CompleteVertex, uint16_t, CompleteVertexHasher> vertex_to_index;

    for (uint16_t i = 0; i < in_vertices.size(); i++) {
        CompleteVertex complete {in_vertices[i], in_uvs[i], in_normals[i]};

        auto find_result = vertex_to_index.find(complete);

        if (find_result != vertex_to_index.end()) {
            // Needed vertex already present in the out_* arguments.
            // Add index only.
            out_indices.push_back(find_result->second);
        }
        else {
            // Needed vertex doesn't present in the out_* arguments.
            // Add the vertex and it's index.
            out_vertices.push_back(complete.vertex);
            out_uvs.push_back(complete.uv);
            out_normals.push_back(complete.normal);

            uint16_t new_index = out_vertices.size() - 1;
            vertex_to_index.insert(std::make_pair(complete, new_index));
            out_indices.push_back(new_index);
        }
    }
}

Mesh::Mesh(std::string mesh_path) {
    std::vector<glm::vec3> unindexed_vertices;
    std::vector<glm::vec2> unindexed_uvs;
    std::vector<glm::vec3> unindexed_normals;

    load_wavefront_obj(mesh_path, unindexed_vertices, unindexed_uvs, unindexed_normals);
    index_vbo(unindexed_vertices, unindexed_uvs, unindexed_normals,
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
