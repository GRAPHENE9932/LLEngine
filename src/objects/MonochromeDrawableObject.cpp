#include "MonochromeDrawableObject.hpp"

MonochromeDrawableObject::MonochromeDrawableObject(glm::vec3 color, std::shared_ptr<Mesh> mesh) :
    color(color) {
        this->mesh = mesh;
}
