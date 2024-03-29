#include "rendering/RenderingServer.hpp"
#include "PBRShaderManager.hpp"
#include "rendering/Skybox.hpp"
#include "rendering/shaders/PBRShader.hpp"

using namespace llengine;

void PBRShaderManager::use_shader(
    RenderingServer& rs,
    const Material& material, const glm::mat4& mvp_matrix,
    const glm::mat4& model_matrix, const glm::vec3& camera_position
) {
    get_shader(rs, material)
        .use_shader(rs, material, mvp_matrix, model_matrix, camera_position);
}

ShaderID PBRShaderManager::get_program_id(RenderingServer& rs, const Material& material) {
    return get_shader(rs, material).get_program_id();
}

const PBRShader& PBRShaderManager::get_shader(
    RenderingServer& rs, const Material& material
) {
    const auto params = PBRShader::to_parameters(rs, material);
    auto iter {pbr_shaders.find(params)};

    if (iter == pbr_shaders.end()) {
        iter = pbr_shaders.emplace(
            PBRShader::to_parameters(rs, material)
        ).first;
    }
    
    return *iter;
}
