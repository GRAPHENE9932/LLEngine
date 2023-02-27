#pragma once

#include "ColoredTextShader.hpp"
#include "EquirectangularMapperShader.hpp"
#include "PBRShaderManager.hpp"
#include "SkyboxShader.hpp"

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

    [[nodiscard]] PBRShaderManager& get_pbr_shader_manager() {
        return pbr_shader_manager.get();
    }

    [[nodiscard]] SkyboxShader& get_skybox_shader() {
        return skybox_shader.get();
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
    LazyShader<PBRShaderManager> pbr_shader_manager;
    LazyShader<SkyboxShader> skybox_shader;
};
