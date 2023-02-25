#include <algorithm>

#include "RenderingServer.hpp"
#include "PBRShaderManager.hpp"
#include "common/core/Skybox.hpp"
#include "common/core/shaders/PBRShader.hpp"

void PBRShaderManager::use_common_shader(
    const Material& material, const glm::mat4& mvp_matrix,
    const glm::mat4& model_matrix, const glm::vec3& camera_position,
    std::optional<std::reference_wrapper<const Texture>> environment_cubemap
) {
    get_common_shader(material, environment_cubemap.has_value())
        .use_shader(material, mvp_matrix, model_matrix, camera_position, environment_cubemap);
}

GLuint PBRShaderManager::get_common_program_id(const Material& material, bool using_environment_cubemap) {
    return get_common_shader(material, using_environment_cubemap).get_program_id();
}

const PBRShader& PBRShaderManager::get_common_shader(const Material& material, bool using_environment_cubemap) {
    const auto params = PBRShader::to_parameters(material, using_environment_cubemap);
    auto iter {common_shaders.find(params)};

    if (iter == common_shaders.end()) {
        iter = common_shaders.emplace(
            PBRShader::to_parameters(material, using_environment_cubemap)
        ).first;
    }
    
    return *iter;
}
