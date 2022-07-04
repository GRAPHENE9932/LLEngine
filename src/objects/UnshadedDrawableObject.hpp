#pragma once

#include <memory>

#include "common/Mesh.hpp"
#include "DrawableObject.hpp"
#include "SpatialObject.hpp"

class UnshadedDrawableObject : public IDrawableObject, public SpatialObject {
public:
    UnshadedDrawableObject(std::shared_ptr<Mesh> mesh, glm::vec3 color);

    glm::vec3 color;
    std::shared_ptr<Mesh> mesh;

    void draw(DrawParameters& params) override;
    GLuint get_program_id(DrawParameters& params) const override;
};
