#pragma once

#include <array>
#include <memory>

#include "common/Texture.hpp"
#include "DrawableObject.hpp"
#include "common/Mesh.hpp"
#include "SpatialObject.hpp"
#include "PointLight.hpp"
#include "SpotLight.hpp"
#include "Camera.hpp"

class TexturedDrawableObject : public IDrawableObject, public SpatialObject {
public:
    std::shared_ptr<Mesh> mesh;
    std::shared_ptr<Texture> texture;

    TexturedDrawableObject(std::shared_ptr<Texture> texture, std::shared_ptr<Mesh> mesh);

    void draw(DrawParameters& params) override;
    GLuint get_program_id(DrawParameters& params) const override;
};
