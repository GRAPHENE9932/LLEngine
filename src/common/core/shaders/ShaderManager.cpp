#include <algorithm>

#include "ShaderManager.hpp"
#include "common/core/shaders/CommonShader.hpp"

ShaderManager::ShaderManager(RenderingServer& rs) :
    rendering_server(rs) {}

void ShaderManager::use_colored_text_shader(const glm::mat4& mvp, const glm::vec3& color) {
    colored_text_shader.use_shader(mvp, color);
}

void ShaderManager::use_skybox_shader(const glm::mat4& mvp) {
    skybox_shader.use_shader(mvp);
}

void ShaderManager::use_common_shader(const Material& material, const glm::mat4& mvp_matrix,
    const glm::mat4& model_matrix) {
    get_common_shader(material)
        .use_shader(material, mvp_matrix, model_matrix);
}

GLuint ShaderManager::get_colored_text_program_id() {
    return colored_text_shader.get_program_id();
}

GLuint ShaderManager::get_skybox_program_id() {
    return skybox_shader.get_program_id();
}

GLuint ShaderManager::get_common_program_id(const Material& material) {
    return get_common_shader(material).get_program_id();
}

const CommonShader& ShaderManager::get_common_shader(const Material& material) {
    const auto params = CommonShader::to_parameters(material, rendering_server);
    auto iter {common_shaders.find(params)};

    if (iter == common_shaders.end()) {
        iter = common_shaders.emplace(
            CommonShader::to_parameters(material, rendering_server),
            rendering_server
        ).first;
    }
    
    return *iter;
}
