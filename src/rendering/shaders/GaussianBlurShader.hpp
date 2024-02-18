#pragma once

#include "rendering/Shader.hpp"
#include "rendering/Texture.hpp"

namespace llengine {
class GaussianBlurShader {
public:
    GaussianBlurShader();

    void use_shader(const Texture& source, const Texture* texture_to_add, float radius, bool vertical) const;

private:
    Shader<"source_texture", "combine", "texture_to_combine", "gauss_weights", "radius", "pixel_width", "is_vertical"> shader;
    Texture gauss_weights;
};
}