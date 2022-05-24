#include <unordered_map>

#include "utils.hpp"

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

void utils::index_vbo(std::vector<glm::vec3>& in_vertices,
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
