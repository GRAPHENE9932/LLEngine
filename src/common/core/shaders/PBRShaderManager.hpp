#pragma once

#include <set> // std::set
#include <type_traits> // std::true_type

#include "EquirectangularMapperShader.hpp"
#include "ColoredTextShader.hpp"
#include "SkyboxShader.hpp"
#include "PBRShader.hpp"

class PBRShaderManager {
public:
    static PBRShaderManager& get_instance() {
        static PBRShaderManager instance;
        return instance;
    }

    void use_shader(
        RenderingServer& rs,
        const Material& material, const glm::mat4& mvp_matrix,
        const glm::mat4& model_matrix, const glm::vec3& camera_position
    );

    GLuint get_program_id(RenderingServer& rs, const Material& material);

private:
    PBRShaderManager() = default;

    const PBRShader& get_shader(RenderingServer& rs, const Material& material);

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
