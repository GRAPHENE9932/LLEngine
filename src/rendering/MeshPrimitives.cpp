#include "rendering/Mesh.hpp"

#include <array>

constexpr std::array<uint16_t, 36> CUBE_INDEXES {
    1, 13, 19,
    1, 19, 8,
    11, 7, 18,
    11, 18, 22,
    23, 20, 12,
    23, 12, 15,
    16, 4, 10,
    16, 10, 21,
    5, 2, 6,
    5, 6, 9,
    17, 14, 0,
    17, 0, 3
};

constexpr std::array<uint16_t, 36> SKYBOX_CUBE_INDEXES {
    1, 19, 13,
    1, 8, 19,
    11, 18, 7,
    11, 22, 18,
    23, 12, 20,
    23, 15, 12,
    16, 10, 4,
    16, 21, 10,
    5, 6, 2,
    5, 9, 6,
    17, 0, 14,
    17, 3, 0
};

constexpr std::array<glm::vec3, 24> CUBE_VERTICES {
    glm::vec3(1.0f, 1.0f, -1.0f),
    glm::vec3(1.0f, 1.0f, -1.0f),
    glm::vec3(1.0f, 1.0f, -1.0f),
    glm::vec3(1.0f, -1.0f, -1.0f),
    glm::vec3(1.0f, -1.0f, -1.0f),
    glm::vec3(1.0f, -1.0f, -1.0f),
    glm::vec3(1.0f, 1.0f, 1.0f),
    glm::vec3(1.0f, 1.0f, 1.0f),
    glm::vec3(1.0f, 1.0f, 1.0f),
    glm::vec3(1.0f, -1.0f, 1.0f),
    glm::vec3(1.0f, -1.0f, 1.0f),
    glm::vec3(1.0f, -1.0f, 1.0f),
    glm::vec3(-1.0f, 1.0f, -1.0f),
    glm::vec3(-1.0f, 1.0f, -1.0f),
    glm::vec3(-1.0f, 1.0f, -1.0f),
    glm::vec3(-1.0f, -1.0f, -1.0f),
    glm::vec3(-1.0f, -1.0f, -1.0f),
    glm::vec3(-1.0f, -1.0f, -1.0f),
    glm::vec3(-1.0f, 1.0f, 1.0f),
    glm::vec3(-1.0f, 1.0f, 1.0f),
    glm::vec3(-1.0f, 1.0f, 1.0f),
    glm::vec3(-1.0f, -1.0f, 1.0f),
    glm::vec3(-1.0f, -1.0f, 1.0f),
    glm::vec3(-1.0f, -1.0f, 1.0f), 
};

constexpr std::array<glm::vec2, 24> CUBE_UVS {
    glm::vec2(0.0f, 0.0f),
    glm::vec2(1.0f, 0.0f),
    glm::vec2(1.0f, 0.0f),
    glm::vec2(0.0f, 1.0f),
    glm::vec2(1.0f, 1.0f),
    glm::vec2(1.0f, 1.0f),
    glm::vec2(0.0f, 0.0f),
    glm::vec2(1.0f, 0.0f),
    glm::vec2(1.0f, 1.0f),
    glm::vec2(0.0f, 1.0f),
    glm::vec2(1.0f, 0.0f),
    glm::vec2(1.0f, 1.0f),
    glm::vec2(0.0f, 0.0f),
    glm::vec2(0.0f, 0.0f),
    glm::vec2(1.0f, 0.0f),
    glm::vec2(0.0f, 1.0f),
    glm::vec2(0.0f, 1.0f),
    glm::vec2(1.0f, 1.0f),
    glm::vec2(0.0f, 0.0f),
    glm::vec2(0.0f, 1.0f),
    glm::vec2(1.0f, 0.0f),
    glm::vec2(0.0f, 0.0f),
    glm::vec2(0.0f, 1.0f),
    glm::vec2(1.0f, 1.0f)
};

constexpr std::array<glm::vec3, 24> CUBE_NORMALS {
    glm::vec3(0.0f, 0.0f, -1.0f),
    glm::vec3(0.0f, 1.0f, 0.0f),
    glm::vec3(1.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 0.0f, 1.0f),
    glm::vec3(0.0f, 1.0f, 0.0f),
    glm::vec3(1.0f, 0.0f, 0.0f),
    glm::vec3(1.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 0.0f, 1.0f),
    glm::vec3(0.0f, 1.0f, 0.0f),
    glm::vec3(1.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 1.0f, 0.0f),
    glm::vec3(0.0f, 0.0f, 1.0f),
    glm::vec3(-1.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 1.0f, 0.0f),
    glm::vec3(0.0f, 0.0f, -1.0f),
    glm::vec3(-1.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, -1.0f, 0.0f),
    glm::vec3(0.0f, 0.0f, -1.0f),
    glm::vec3(0.0f, 0.0f, 1.0f),
    glm::vec3(0.0f, 1.0f, 0.0f),
    glm::vec3(-1.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, -1.0f, 0.0f),
    glm::vec3(0.0f, 0.0f, 1.0f),
    glm::vec3(-1.0f, 0.0f, 0.0f)
};

constexpr std::array<glm::vec4, 24> CUBE_TANGENTS {
    glm::vec4(-1.0f, 0.0f, 0.0f, 1.0f),
    glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
    glm::vec4(0.0f, 0.0f, -1.0f, 1.0f),
    glm::vec4(-1.0f, 0.0f, 0.0f, 1.0f),
    glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
    glm::vec4(0.0f, 0.0f, -1.0f, 1.0f),
    glm::vec4(0.0f, 0.0f, -1.0f, 1.0f),
    glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
    glm::vec4(1.0f, 0.0f, 1.0f, 1.0f),
    glm::vec4(0.0f, 0.0f, -1.0f, 1.0f),
    glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
    glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
    glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
    glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
    glm::vec4(-1.0f, 0.0f, 0.0f, 1.0f),
    glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
    glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
    glm::vec4(-1.0f, 0.0f, 0.0f, 1.0f),
    glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
    glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
    glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
    glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
    glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
    glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)
};

std::shared_ptr<Mesh> cached_cube = nullptr;
[[nodiscard]] std::shared_ptr<const Mesh> Mesh::get_cube() {
    if (!cached_cube) {
        cached_cube = std::make_shared<Mesh>();
        cached_cube->set_indices(std::vector(CUBE_INDEXES.begin(), CUBE_INDEXES.end()));
        cached_cube->set_vertices(std::vector(CUBE_VERTICES.begin(), CUBE_VERTICES.end()));
        cached_cube->set_uvs(std::vector(CUBE_UVS.begin(), CUBE_UVS.end()));
        cached_cube->set_normals(std::vector(CUBE_NORMALS.begin(), CUBE_NORMALS.end()));
        cached_cube->set_tangents(std::vector(CUBE_TANGENTS.begin(), CUBE_TANGENTS.end()));
    }

    return cached_cube;
}

std::shared_ptr<Mesh> cached_skybox_cube = nullptr;
[[nodiscard]] std::shared_ptr<const Mesh> Mesh::get_skybox_cube() {
    if (!cached_skybox_cube) {
        cached_skybox_cube = std::make_shared<Mesh>();
        cached_skybox_cube->set_indices(std::vector(SKYBOX_CUBE_INDEXES.begin(), SKYBOX_CUBE_INDEXES.end()));
        cached_skybox_cube->set_vertices(std::vector(CUBE_VERTICES.begin(), CUBE_VERTICES.end()));
    }

    return cached_skybox_cube;
}

constexpr std::array<glm::vec3, 6> QUAD_VERTICES {
    glm::vec3(1.0f, 1.0f, 0.0f),
    glm::vec3(-1.0f, -1.0f, 0.0f),
    glm::vec3(1.0f, -1.0f, 0.0f),
    glm::vec3(-1.0f, -1.0f, 0.0f),
    glm::vec3(1.0f, 1.0f, 0.0f),
    glm::vec3(-1.0f, 1.0f, 0.0f)
};

constexpr std::array<glm::vec2, 6> QUAD_UVS {
    glm::vec2(1.0f, 1.0f),
    glm::vec2(0.0f, 0.0f),
    glm::vec2(1.0f, 0.0f),
    glm::vec2(0.0f, 0.0f),
    glm::vec2(1.0f, 1.0f),
    glm::vec2(0.0f, 1.0f)
};

std::shared_ptr<Mesh> cached_quad = nullptr;
[[nodiscard]] std::shared_ptr<const Mesh> Mesh::get_quad() {
    if (!cached_quad) {
        cached_quad = std::make_shared<Mesh>();
        cached_quad->set_vertices(std::vector(QUAD_VERTICES.begin(), QUAD_VERTICES.end()));
        cached_quad->set_uvs(std::vector(QUAD_UVS.begin(), QUAD_UVS.end()));
    }

    return cached_quad;
}