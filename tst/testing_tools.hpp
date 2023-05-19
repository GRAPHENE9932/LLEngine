#pragma once

#include <gtest/gtest.h>
#include <glm/vec3.hpp>
#include <glm/gtx/quaternion.hpp>

#include <string>
#include <concepts>
#include <span>
#include <fstream>
#include <algorithm>

inline std::string create_temporary_file(const std::span<const unsigned char>& data, const std::string& file_name) {
    std::string path = testing::TempDir() + file_name;
    std::ofstream stream(path, std::ios::out | std::ios::binary);
    stream.write(reinterpret_cast<const char*>(data.data()), data.size());
    return path;
}

template<std::input_iterator IterM, std::input_iterator IterT>
[[nodiscard]] inline IterM find_triangle(
    IterM mesh_begin, IterM mesh_end,
    IterT triangle_begin, IterT triangle_end
) {
    for (; mesh_begin != mesh_end; mesh_begin += 3) {
        bool triangle_found =
            *(mesh_begin + 0) == *(triangle_begin + 0) &&
            *(mesh_begin + 1) == *(triangle_begin + 1) &&
            *(mesh_begin + 2) == *(triangle_begin + 2) ||
            *(mesh_begin + 0) == *(triangle_begin + 1) &&
            *(mesh_begin + 1) == *(triangle_begin + 2) &&
            *(mesh_begin + 2) == *(triangle_begin + 0) ||
            *(mesh_begin + 0) == *(triangle_begin + 2) &&
            *(mesh_begin + 1) == *(triangle_begin + 0) &&
            *(mesh_begin + 2) == *(triangle_begin + 1);
        
        if (triangle_found) {
            return mesh_begin;
        }
    }

    return mesh_end;
}

[[nodiscard]] inline bool compare_triangular_meshes(
    const std::span<const glm::vec3>& vertices_1,
    const std::span<const glm::vec3>& vertices_2
) {
    if (vertices_1.size() != vertices_2.size()) {
        return false;
    }

    std::vector<glm::vec3> vertices_2_copy(vertices_2.begin(), vertices_2.end());
    for (std::size_t i = 0; i < vertices_1.size(); i += 3) {
        auto search_result = find_triangle(
            vertices_2_copy.begin(), vertices_2_copy.end(),
            vertices_1.begin() + i, vertices_1.begin() + 3
        );

        if (search_result == vertices_2_copy.end()) {
            return false;
        }

        vertices_2_copy.erase(search_result, search_result + 3);
    }

    return true;
}

template<std::integral IndexT>
[[nodiscard]] inline bool compare_triangular_meshes(
    const std::span<const IndexT>& indices_1,
    const std::span<const glm::vec3>& vertices_1,
    const std::span<const glm::vec3>& vertices_2
) {
    std::vector<glm::vec3> unindexed_vertices_1;
    for (IndexT index : indices_1) {
        unindexed_vertices_1.push_back(vertices_1[index]);
    }

    return compare_triangular_meshes(unindexed_vertices_1, vertices_2);
}

inline void expect_near_quat(const glm::quat& quat_1, const glm::quat& quat_2, float error) {
    EXPECT_NEAR(quat_1.w, quat_2.w, error);
    EXPECT_NEAR(quat_1.x, quat_2.x, error);
    EXPECT_NEAR(quat_1.y, quat_2.y, error);
    EXPECT_NEAR(quat_1.z, quat_2.z, error);
}

inline void expect_near_vec3(const glm::vec3& vec_1, const glm::vec3& vec_2, float error) {
    EXPECT_NEAR(vec_1.x, vec_2.x, error);
    EXPECT_NEAR(vec_1.y, vec_2.y, error);
    EXPECT_NEAR(vec_1.z, vec_2.z, error);
}