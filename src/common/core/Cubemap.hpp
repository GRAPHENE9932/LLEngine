#pragma once

#include <memory> // std::shared_ptr

#include <GL/glew.h> // GLuint

class Texture;

class Cubemap {
public:
    Cubemap(const Cubemap& other) = delete;
    Cubemap(Cubemap&& other) noexcept;

    static Cubemap from_cubemap(const std::shared_ptr<Texture>& cubemap_texture);
    static Cubemap from_panorama(const std::shared_ptr<Texture>& panorama_texture);

    const Texture& get_texture() {
        return *cubemap_texture;
    }

    void draw();

private:
    std::shared_ptr<Texture> cubemap_texture;

    Cubemap(const std::shared_ptr<Texture>& cubemap_texture);
};
