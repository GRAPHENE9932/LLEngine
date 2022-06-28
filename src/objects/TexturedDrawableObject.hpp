#pragma once

#include <array>
#include <memory>

#include "../common/Texture.hpp"
#include "DrawableObject.hpp"
#include "../common/Mesh.hpp"
#include "SpatialObject.hpp"
#include "PointLight.hpp"
#include "SpotLight.hpp"
#include "Camera.hpp"

class TexturedDrawableObject : public DrawableObject, public SpatialObject {
public:
    static GLuint program_id;

    std::shared_ptr<Mesh> mesh;
    std::shared_ptr<Texture> texture;

    TexturedDrawableObject(std::shared_ptr<Texture> texture, std::shared_ptr<Mesh> mesh);

    static void pre_init();
    static void clean_up();

    void draw(DrawParameters& params) override;
    GLuint get_program_id() const override {
        return program_id;
    }

private:
    static GLuint mvp_matrix_uniform_id, model_matrix_uniform_id,
        normal_matrix_uniform_id, light_position_uniform_id;
    static std::array<PointLight::Uniforms, POINT_LIGHTS_AMOUNT> point_light_uniforms;
    static std::array<SpotLight::Uniforms, SPOT_LIGHTS_AMOUNT> spot_light_uniforms;
};
