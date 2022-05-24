#pragma once

#include <vector>
#include <memory>

#include <GL/glew.h>

#include "../common/Mesh.hpp"
#include "SpatialObject.hpp"

class DrawableObject : public SpatialObject {
public:
    std::shared_ptr<Mesh> mesh;
};
