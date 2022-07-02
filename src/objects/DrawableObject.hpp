#pragma once

#include <array>

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <GL/glew.h>

#include "PointLight.hpp"
#include "SpotLight.hpp"

struct DrawParameters {
    std::array<PointLight, POINT_LIGHTS_AMOUNT> point_lights;
    std::array<SpotLight, SPOT_LIGHTS_AMOUNT> spot_lights;
    glm::mat4 view_proj_matrix, view_matrix, proj_matrix;
    GLuint cur_shader;
    bool overlay_mode;
    unsigned long triangles_drawn;
};

class IDrawableObject {
public:
    virtual void draw(DrawParameters& params) = 0;
    virtual GLuint get_program_id() const = 0;
    virtual ~IDrawableObject() = default;
};
