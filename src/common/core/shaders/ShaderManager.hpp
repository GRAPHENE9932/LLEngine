#pragma once

#include <set> // std::set
#include <type_traits> // std::true_type

#include "EquirectangularMapperShader.hpp"
#include "ColoredTextShader.hpp"
#include "SkyboxShader.hpp"
#include "CommonShader.hpp"

class ShaderManager {
public:
    void use_equirectangular_mapper_shader(
        const glm::mat4& mvp,
        GLuint panorama_texture_id
    );
    void use_colored_text_shader(const glm::mat4& mvp, const glm::vec3& color);
    void use_skybox_shader(const glm::mat4& mvp);
    void use_common_shader(
        const Material& material, const glm::mat4& mvp_matrix,
        const glm::mat4& model_matrix, const glm::vec3& camera_position,
        std::optional<std::reference_wrapper<const Texture>> environment_cubemap
    );

    GLuint get_colored_text_program_id();
    GLuint get_skybox_program_id();
    GLuint get_common_program_id(const Material& material, bool using_environment_cubemap);

private:
    const CommonShader& get_common_shader(const Material& material, bool using_environment_cubemap);

    EquirectangularMapperShader equirectangular_mapper_shader;
    ColoredTextShader colored_text_shader;
    SkyboxShader skybox_shader;

    struct ComShaderComparator {
        using is_transparent = std::true_type;

        bool operator()(const CommonShader& left, const CommonShader& right) const noexcept {
            return left.extract_parameters() < right.extract_parameters();
        }
        bool operator()(const CommonShader& left, const CommonShader::Parameters& right) const noexcept {
            return left.extract_parameters() < right;
        }
        bool operator()(const CommonShader::Parameters& left, const CommonShader& right) const noexcept {
            return left < right.extract_parameters();
        }
    };
    std::set<CommonShader, ComShaderComparator> common_shaders;
};
