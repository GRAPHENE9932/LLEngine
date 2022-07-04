#pragma once

#include <vector>

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include "objects/SpotLight.hpp"
#include "objects/PointLight.hpp"

class TexturedShader {
public:
    ~TexturedShader();

    void initialize(const std::size_t spot_lights_count,
                    const std::size_t point_lights_count);
    void use_shader(const glm::mat4& mvp, const glm::mat4& model_matrix,
                    const bool overlay,
                    std::vector<SpotLight>& spot_lights,
                    std::vector<PointLight>& point_lights);
    GLuint get_program_id(std::size_t spot_lights_count, std::size_t point_lights_count);
    void delete_shader();

    inline bool is_initialized() const noexcept {
        return program_id != 0;
    }
    inline std::size_t get_spot_lights_count() const noexcept {
        return spot_light_ids.size();
    }
    inline std::size_t get_point_lights_count() const noexcept {
        return point_light_ids.size();
    }

private:
    GLuint program_id = 0;
    GLuint mvp_id, model_matrix_id, normal_matrix_id;
    std::vector<SpotLight::Uniforms> spot_light_ids;
    std::vector<PointLight::Uniforms> point_light_ids;
};
