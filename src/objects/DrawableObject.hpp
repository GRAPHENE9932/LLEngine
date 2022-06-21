#pragma once

#include <array>

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <GL/glew.h>

#include "PointLight.hpp"

struct EnvironmentInfo {
    std::array<PointLight, POINT_LIGHTS_AMOUNT> point_lights;
    glm::vec3 camera_direction;
    GLuint cur_shader;
};

class DrawableObject {
public:
    virtual void draw(const glm::mat4& vp, EnvironmentInfo& env_info) = 0;
    virtual GLuint get_program_id() const = 0;
};
