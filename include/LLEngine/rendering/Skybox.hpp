#pragma once

#include "rendering/Shader.hpp"
#include "rendering/LazyShader.hpp"

#include <memory> // std::shared_ptr

namespace llengine {
class Texture;
class RenderingServer;

class Skybox {
public:
    Skybox(const std::shared_ptr<Texture>& cubemap_texture);
    Skybox(const Skybox& other) = delete;
    Skybox(Skybox&& other) noexcept;
    Skybox& operator=(const Skybox& other) = delete;
    Skybox& operator=(Skybox&& other) noexcept;

    const Texture& get_texture() {
        return *cubemap_texture;
    }

    void draw(RenderingServer& rs);

private:
    using ShaderType = Shader<"mvp">;

    std::shared_ptr<Texture> cubemap_texture;
    static LazyShader<ShaderType> shader;
};
}