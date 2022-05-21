#pragma once

#include <glm/vec3.hpp>

#include "../common/Mesh.hpp"
#include "DrawableObject.hpp"

class MonochromeDrawableObject : public DrawableObject {
public:
    glm::vec3 color;

    MonochromeDrawableObject(glm::vec3 color, std::shared_ptr<Mesh> mesh);
};
