#pragma once

#include <memory> // std::shared_ptr

#include <GL/glew.h> // GLuint

class Texture;

class Skybox {
public:
    explicit Skybox(const std::shared_ptr<Texture>& cubemap_texture);
    Skybox(const Skybox& other) = delete;
    Skybox(Skybox&& other) noexcept;
    Skybox& operator=(const Skybox& other) = delete;

    const Texture& get_texture() {
        return *cubemap_texture;
    }

    void draw();

private:
    std::shared_ptr<Texture> cubemap_texture;
};
