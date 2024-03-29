#pragma once

#include <optional>
#include <string_view>
#include <vector>
#include <string>

namespace llengine {
template<typename ShaderT>
class LazyShader {
public:
    LazyShader(
        std::string_view vertex_shader_code, std::string_view fragment_shader_code,
        const std::vector<std::string>& defines = {}
    ) : vertex_shader_code(vertex_shader_code), fragment_shader_code(fragment_shader_code), defines(defines) {

    }

    [[nodiscard]] ShaderT& get() {
        if (!shader.has_value()) {
            initialize_shader();
        }
        return *shader;
    }

    [[nodiscard]] const ShaderT& get() const {
        if (!shader.has_value()) {
            initialize_shader();
        }
        return *shader;
    }

    [[nodiscard]] const ShaderT& operator*() const {
        return get();
    }

    [[nodiscard]] ShaderT& operator*() {
        return get();
    }

    [[nodiscard]] const ShaderT* operator->() const {
        return &get();
    }

    [[nodiscard]] ShaderT* operator->() {
        return &get();
    }

    [[nodiscard]] bool is_initialized() const {
        return shader.has_value();
    }

private:
    std::string_view vertex_shader_code;
    std::string_view fragment_shader_code;
    std::vector<std::string> defines;

    mutable std::optional<ShaderT> shader;

    void initialize_shader() {
        shader.emplace(vertex_shader_code, fragment_shader_code, defines);
        defines.clear(); // Clear the defines as they are no longer needed.
    }
};
}
