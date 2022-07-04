#include <algorithm>

#include "ShaderManager.hpp"

void ShaderManager::use_colored_text_shader(const glm::mat4& mvp, const glm::vec3& color) {
    colored_text_shader.use_shader(mvp, color);
}

void ShaderManager::use_skybox_shader(const glm::mat4& mvp) {
    skybox_shader.use_shader(mvp);
}

void ShaderManager::use_textured_shader(const glm::mat4& mvp, const glm::mat4& model_matrix,
                                        const bool overlay,
                                        std::vector<SpotLight>& spot_lights,
                                        std::vector<PointLight>& point_lights) {
    // Find the textured shader with the same amount of spot and point lights.
    // If not found, then create new.
    auto iter {std::find_if(
        textured_shaders.begin(),
        textured_shaders.end(),
        [&spot_lights, &point_lights](const TexturedShader& shader) {
            return shader.get_spot_lights_count() == spot_lights.size() &&
                   shader.get_point_lights_count() == point_lights.size();
        }
    )};
    if (iter == textured_shaders.end()) {
        textured_shaders.push_back({});
        iter = textured_shaders.end() - 1;
    }

    iter->use_shader(mvp, model_matrix, overlay, spot_lights, point_lights);
}

void ShaderManager::use_unshaded_shader(const glm::mat4& mvp, const glm::vec3& color) {
    unshaded_shader.use_shader(mvp, color);
}

void ShaderManager::use_unshaded_textured_shader(const glm::mat4& mvp) {
    unshaded_textured_shader.use_shader(mvp);
}

GLuint ShaderManager::get_colored_text_program_id() {
    return colored_text_shader.get_program_id();
}

GLuint ShaderManager::get_skybox_program_id() {
    return skybox_shader.get_program_id();
}

GLuint ShaderManager::get_textured_program_id(std::size_t spot_lights_count, std::size_t point_lights_count) {
    // Find the textured shader with the same amount of spot and point lights.
    // If not found, then create new.
    auto iter {std::find_if(
        textured_shaders.begin(),
        textured_shaders.end(),
        [&spot_lights_count, &point_lights_count](const TexturedShader& shader) {
            return shader.get_spot_lights_count() == spot_lights_count &&
                   shader.get_point_lights_count() == point_lights_count;
        }
    )};
    if (iter == textured_shaders.end()) {
        textured_shaders.push_back({});
        iter = textured_shaders.end() - 1;
    }

    return iter->get_program_id(spot_lights_count, point_lights_count);
}

GLuint ShaderManager::get_unshaded_program_id() {
    return unshaded_shader.get_program_id();
}

GLuint ShaderManager::get_unshaded_textured_program_id() {
    return unshaded_textured_shader.get_program_id();
}

void ShaderManager::initialize_all() {
    colored_text_shader.initialize();
    skybox_shader.initialize();
    unshaded_shader.initialize();
    unshaded_textured_shader.initialize();
}
