#include "PBRShaderManager.hpp"
#include "rendering/shaders/PBRShader.hpp"

using namespace llengine;

void PBRShaderManager::use_shader(
    const Material& material, const glm::mat4& mvp_matrix,
    const glm::mat4& model_matrix, const glm::vec3& camera_position
) {
    get_shader(material)
        .use_shader(material, mvp_matrix, model_matrix, camera_position);
}

ShaderID PBRShaderManager::get_program_id(const Material& material) {
    return get_shader(material).get_program_id();
}

const PBRShader& PBRShaderManager::get_shader(const Material& material) {
    const auto params = PBRShader::to_parameters(material);
    auto iter {pbr_shaders.find(params)};

    if (iter == pbr_shaders.end()) {
        iter = pbr_shaders.emplace(
            PBRShader::to_parameters(material)
        ).first;
    }
    
    return *iter;
}
