#pragma once

#include "rendering/Shader.hpp"
#include "rendering/Texture.hpp"

namespace llengine {
class GaussianBlurShader {
public:
    GaussianBlurShader();

    void use_shader(const Texture& source, float radius, bool vertical) const;

private:
    Shader<"source_texture", "gauss_weights", "radius", "pixel_width", "is_vertical"> shader;
    Texture gauss_weights;
};
}