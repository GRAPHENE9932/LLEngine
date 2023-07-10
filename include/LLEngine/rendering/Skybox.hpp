#pragma once

#include <memory> // std::shared_ptr

#include <GL/glew.h> // GLuint

namespace llengine {
class Texture;
class RenderingServer;

class Skybox {
public:
    Skybox(RenderingServer& rs, const std::shared_ptr<Texture>& cubemap_texture);
    Skybox(const Skybox& other) = delete;
    Skybox(Skybox&& other) noexcept;
    Skybox& operator=(const Skybox& other) = delete;
    Skybox& operator=(Skybox&& other) = delete;

    const Texture& get_texture() {
        return *cubemap_texture;
    }

    void draw();

private:
    RenderingServer& rs;
    std::shared_ptr<Texture> cubemap_texture;
};
}