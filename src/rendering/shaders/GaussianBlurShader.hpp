#pragma once

#include "rendering/Shader.hpp"
#include "rendering/Texture.hpp"

namespace llengine {
class GaussianBlurShader {
public:
    GaussianBlurShader();

    void use_horizontal_shader(const Texture& source, float radius, float step_coefficient = 1.0f) const;
    void use_vertical_shader(const Texture& source, float radius, float step_coefficient = 1.0f) const;

private:
    Shader<"source_texture", "gauss_weights", "radius", "hor_step", "coefficient", "samples_to_take"> hor_blur_shader;
    Shader<"source_texture", "gauss_weights", "radius", "vert_step", "coefficient"> vert_blur_shader;
    Texture gauss_weights;
};
}