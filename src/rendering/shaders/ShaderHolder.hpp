#pragma once

#include "PBRShaderManager.hpp"

namespace llengine {
/**
 * @brief Stores all kinds of shaders and shader managers.
 *
 * The shaders you get may be lazily initialized.
 */
class ShaderHolder {
public:
    [[nodiscard]] PBRShaderManager& get_pbr_shader_manager() {
        return pbr_shader_manager.get();
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

    LazyShader<PBRShaderManager> pbr_shader_manager;
};
}