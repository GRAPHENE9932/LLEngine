#include <stdexcept>

#include <glm/gtc/type_ptr.hpp>

#include "utils/shader_loader.hpp"
#include "TexturedShader.hpp"

TexturedShader::~TexturedShader() {
    delete_shader();
}

void TexturedShader::initialize(const std::size_t spot_lights_count,
                                const std::size_t point_lights_count) {
    program_id = load_shaders(
        "res/shaders/textured_vertex.glsl",
        "res/shaders/textured_fragment.glsl",
        {
            "SPOT_LIGHTS_COUNT " + std::to_string(spot_lights_count),
            "POINT_LIGHTS_COUNT " + std::to_string(point_lights_count)
        }
    );
    // Init uniforms.
    mvp_id = glGetUniformLocation(program_id, "MVP");
    if (spot_lights_count != 0 || point_lights_count != 0) {
        model_matrix_id = glGetUniformLocation(program_id, "MODEL_MATRIX");
        normal_matrix_id = glGetUniformLocation(program_id, "NORMAL_MATRIX");
    }

    spot_light_ids.resize(spot_lights_count);
    for (GLuint i = 0; i < spot_lights_count; i++)
        spot_light_ids[i] = SpotLight::get_uniforms_id(program_id, "SPOT_LIGHTS", i);

    point_light_ids.resize(point_lights_count);
    for (GLuint i = 0; i < point_lights_count; i++)
        point_light_ids[i] = PointLight::get_uniforms_id(program_id, "POINT_LIGHTS", i);
}

void TexturedShader::use_shader(const glm::mat4& mvp, const glm::mat4& model_matrix,
                                const bool overlay,
                                std::vector<std::shared_ptr<SpotLight>>& spot_lights,
                                std::vector<std::shared_ptr<PointLight>>& point_lights) {
    if (!is_initialized())
        initialize(spot_lights.size(), point_lights.size());

    if (get_spot_lights_count() != spot_lights.size())
        throw std::invalid_argument("Invalid count of spot lights provided.");
    if (get_point_lights_count() != point_lights.size())
        throw std::invalid_argument("Invalid count of point lights provided.");

    glUseProgram(program_id);

    glUniformMatrix4fv(mvp_id, 1, GL_FALSE, glm::value_ptr(mvp));
    if (get_spot_lights_count() != 0 || get_point_lights_count() != 0) {
        glUniformMatrix4fv(model_matrix_id, 1, GL_FALSE, glm::value_ptr(model_matrix));
        glm::mat4 normal_matrix = glm::transpose(glm::inverse(model_matrix));
        glUniformMatrix4fv(normal_matrix_id, 1, GL_FALSE, glm::value_ptr(normal_matrix));
    }

    for (GLuint i = 0; i < spot_lights.size(); i++)
        spot_lights[i]->set_uniforms(spot_light_ids[i], overlay);
    for (GLuint i = 0; i < point_lights.size(); i++)
        point_lights[i]->set_uniforms(point_light_ids[i], overlay);
}

GLuint TexturedShader::get_program_id(std::size_t spot_lights_count,
                                      std::size_t point_lights_count) {
    if (!is_initialized())
        initialize(spot_lights_count, point_lights_count);

    return program_id;
}

void TexturedShader::delete_shader() {
    if (is_initialized()) {
        glDeleteProgram(program_id);
        program_id = 0;
    }
}
