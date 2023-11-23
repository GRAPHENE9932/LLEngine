#pragma once

#include "AABB.hpp"
#include "Plane.hpp"

#include <glm/geometric.hpp>
#include <glm/vec3.hpp>

namespace llengine {
struct Frustum {
    struct {
        Plane top, bottom, left, right, far, near;

        [[nodiscard]] const Plane& operator[](std::size_t index) const {
            assert(index < 6);

            switch (index) {
            case 0: return top;
            case 1: return bottom;
            case 2: return left;
            case 3: return right;
            case 4: return far;
            case 5: return near;
            default: return top;
            }
        }

        [[nodiscard]] Plane& operator[](std::size_t index) {
            assert(index < 6);

            switch (index) {
            case 0: return top;
            case 1: return bottom;
            case 2: return left;
            case 3: return right;
            case 4: return far;
            case 5: return near;
            default: return top;
            }
        }
    } planes;

    [[nodiscard]] bool is_aabb_on_frustum(const AABB& aabb) const {
        for (std::size_t plane_i = 0; plane_i < 6; plane_i++) {
            bool has_vertices_facing_the_plane = false;

            for (std::size_t vertex_i = 0; vertex_i < 8; vertex_i++) {
                if (planes[plane_i].is_facing_the_point(aabb.get_vertex(vertex_i))) {
                    has_vertices_facing_the_plane = true;
                }
            }

            if (!has_vertices_facing_the_plane) {
                return false;
            }
        }

        return true;
    }
};
}