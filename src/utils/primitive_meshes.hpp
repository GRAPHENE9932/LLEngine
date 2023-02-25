#pragma once

#include "common/core/Mesh.hpp"

#include <memory>

namespace primitives {
    /**
     * @brief Get the indexed 2x2x2 cube mesh with UVs, normals and tangents.
     *
     * The Mesh object is being created only on the first call, after that
     * returns shared_ptr with the same object.
     */
    std::shared_ptr<const Mesh> get_cube();

    /**
     * @brief Get the indexed 2x2x2 cube mesh that can be used as skybox mesh.
     * 
     * This Mesh DOES NOT contain UVs, normals or tangents. Triangles are
     * faced inwards, so with enabled backface culling the cube will be
     * visible only from inside.
     */
    std::shared_ptr<const Mesh> get_skybox_cube();
}