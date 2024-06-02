#pragma once

#include <set> // std::set
#include <type_traits> // std::true_type

#include "PBRShader.hpp"
#include "datatypes.hpp"

namespace llengine {
class PBRShaderManager {
public:
    PBRShaderManager() = default;

    void use_shader(
        const Material& material, const glm::mat4& mvp_matrix,
        const glm::mat4& model_matrix, const glm::vec3& camera_position
    );

    ShaderID get_program_id(const Material& material);

private:
    const PBRShader& get_shader(const Material& material);

    struct PBRShaderComparator {
        using is_transparent = std::true_type;

        bool operator()(const PBRShader& left, const PBRShader& right) const noexcept {
            return left.extract_parameters() < right.extract_parameters();
        }
        bool operator()(const PBRShader& left, const PBRShader::Parameters& right) const noexcept {
            return left.extract_parameters() < right;
        }
        bool operator()(const PBRShader::Parameters& left, const PBRShader& right) const noexcept {
            return left < right.extract_parameters();
        }
    };
    std::set<PBRShader, PBRShaderComparator> pbr_shaders;
};
}