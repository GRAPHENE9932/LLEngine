#pragma once

#include <memory>

#include "../common/Texture.hpp"
#include "DrawableObject.hpp"

class SkyboxObject : public IDrawableObject {
public:
    std::shared_ptr<Texture> texture;

    SkyboxObject(const std::shared_ptr<Texture>& texture) : texture(texture) {};

    static void pre_init();
    static void clean_up();

    void draw(DrawParameters& params) override;
    inline GLuint get_program_id(DrawParameters& params) const override;

private:
    static GLuint vertices_id;
};
