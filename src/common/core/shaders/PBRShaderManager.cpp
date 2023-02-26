#include <algorithm>

#include "RenderingServer.hpp"
#include "PBRShaderManager.hpp"
#include "common/core/Skybox.hpp"
#include "common/core/shaders/PBRShader.hpp"

void PBRShaderManager::use_common_shader(
    RenderingServer& rs,
    const Material& material, const glm::mat4& mvp_matrix,
    const glm::mat4& model_matrix, const glm::vec3& camera_position
) {
    get_common_shader(rs, material)
        .use_shader(rs, material, mvp_matrix, model_matrix, camera_position);
}

GLuint PBRShaderManager::get_common_program_id(RenderingServer& rs, const Material& material) {
    return get_common_shader(rs, material).get_program_id();
}

const PBRShader& PBRShaderManager::get_common_shader(
    RenderingServer& rs, const Material& material
) {
    const auto params = PBRShader::to_parameters(rs, material);
    auto iter {common_shaders.find(params)};

    if (iter == common_shaders.end()) {
        iter = common_shaders.emplace(
            PBRShader::to_parameters(rs, material)
        ).first;
    }
    
    return *iter;
}
