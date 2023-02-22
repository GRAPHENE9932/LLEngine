#include <algorithm>

#include "ShaderManager.hpp"
#include "common/core/Cubemap.hpp"
#include "common/core/shaders/CommonShader.hpp"

ShaderManager::ShaderManager(RenderingServer& rs) :
    rendering_server(rs) {}

void ShaderManager::use_equirectangular_mapper_shader(
    const glm::mat4& mvp,
    GLuint panorama_texture_id
) {
    equirectangular_mapper_shader.use_shader(mvp, panorama_texture_id);
}

void ShaderManager::use_colored_text_shader(const glm::mat4& mvp, const glm::vec3& color) {
    colored_text_shader.use_shader(mvp, color);
}

void ShaderManager::use_skybox_shader(const glm::mat4& mvp) {
    skybox_shader.use_shader(mvp);
}

void ShaderManager::use_common_shader(
    const Material& material, const glm::mat4& mvp_matrix,
    const glm::mat4& model_matrix, const glm::vec3& camera_position,
    std::optional<std::reference_wrapper<Cubemap>> environment_cubemap
) {
    get_common_shader(material, environment_cubemap.has_value())
        .use_shader(material, mvp_matrix, model_matrix, camera_position, environment_cubemap);
}

GLuint ShaderManager::get_colored_text_program_id() {
    return colored_text_shader.get_program_id();
}

GLuint ShaderManager::get_skybox_program_id() {
    return skybox_shader.get_program_id();
}

GLuint ShaderManager::get_common_program_id(const Material& material, bool using_environment_cubemap) {
    return get_common_shader(material, using_environment_cubemap).get_program_id();
}

const CommonShader& ShaderManager::get_common_shader(const Material& material, bool using_environment_cubemap) {
    const auto params = CommonShader::to_parameters(material, using_environment_cubemap, rendering_server);
    auto iter {common_shaders.find(params)};

    if (iter == common_shaders.end()) {
        iter = common_shaders.emplace(
            CommonShader::to_parameters(material, using_environment_cubemap, rendering_server),
            rendering_server
        ).first;
    }
    
    return *iter;
}
