#pragma once

#include <array>

#include "../utils/utils.hpp"
#include "PointLight.hpp"
#include "DrawableObject.hpp"

// Also, change src/shaders/textured_fragment.glsl:TX_DRW_POINT_LIGHTS_AMOUNT
constexpr GLuint TX_DRW_POINT_LIGHTS_AMOUNT = 4;

class TexturedDrawableObject : public DrawableObject {
public:
    static GLuint program_id;

    std::shared_ptr<utils::ManagedTextureID> texture_id;

    TexturedDrawableObject(std::shared_ptr<utils::ManagedTextureID> texture_id,
        std::shared_ptr<Mesh> mesh);
    ~TexturedDrawableObject();

    static void pre_init();
    static void clean_up();

    void draw(GLfloat* camera_mvp, GLfloat* camera_dir,
              const std::array<PointLight, TX_DRW_POINT_LIGHTS_AMOUNT>& point_lights);

private:
    static GLuint mvp_matrix_uniform_id, model_matrix_uniform_id,
        normal_matrix_uniform_id, camera_direction_uniform_id,
        light_position_uniform_id;
    static std::array<PointLight::Uniforms, TX_DRW_POINT_LIGHTS_AMOUNT> point_light_uniforms;
};