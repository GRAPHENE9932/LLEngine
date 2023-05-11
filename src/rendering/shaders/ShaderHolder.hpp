#pragma once

#include "ColoredTextShader.hpp"
#include "EquirectangularMapperShader.hpp"
#include "PBRShaderManager.hpp"
#include "SkyboxShader.hpp"
#include "IrradiancePrecomputerShader.hpp"
#include "SpecularPrefilterShader.hpp"
#include "BRDFIntegrationMapperShader.hpp"
#include "GUIRectangleShader.hpp"

/**
 * @brief Stores all kinds of shaders and shader managers.
 * 
 * The shaders you get may be lazily initialized.
 */
class ShaderHolder {
public:
    [[nodiscard]] ColoredTextShader& get_colored_text_shader() {
        return colored_text_shader.get();
    }

    [[nodiscard]] EquirectangularMapperShader& get_equirectangular_mapper_shader() {
        return equirectangular_mapper_shader.get();
    }

    [[nodiscard]] IrradiancePrecomputerShader& get_irradiance_precomputer_shader() {
        return irradiance_precomputer_shader.get();
    }

    [[nodiscard]] BRDFIntegrationMapperShader& get_brdf_integration_mapper_shader() {
        return brdf_integration_mapper_shader.get();
    }

    [[nodiscard]] SpecularPrefilterShader& get_specular_prefilter_shader() {
        return specular_prefilter_shader.get();
    }

    [[nodiscard]] PBRShaderManager& get_pbr_shader_manager() {
        return pbr_shader_manager.get();
    }

    [[nodiscard]] SkyboxShader& get_skybox_shader() {
        return skybox_shader.get();
    }

    [[nodiscard]] GUIRectangleShader& get_gui_rectangle_shader() {
        return gui_rectangle_shader.get();
    }

private:
    template<typename T>
    class LazyShader {
    public:
        T& get() {
            if (!shader) [[unlikely]] {
                shader = std::make_unique<T>();
            }
            return *shader;
        }

    private:
        std::unique_ptr<T> shader = nullptr;
    };

    LazyShader<ColoredTextShader> colored_text_shader;
    LazyShader<EquirectangularMapperShader> equirectangular_mapper_shader;
    LazyShader<IrradiancePrecomputerShader> irradiance_precomputer_shader;
    LazyShader<BRDFIntegrationMapperShader> brdf_integration_mapper_shader;
    LazyShader<SpecularPrefilterShader> specular_prefilter_shader;
    LazyShader<PBRShaderManager> pbr_shader_manager;
    LazyShader<SkyboxShader> skybox_shader;
    LazyShader<GUIRectangleShader> gui_rectangle_shader;
};
