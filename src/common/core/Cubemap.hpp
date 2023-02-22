#pragma once

#include <memory> // std::shared_ptr

#include <GL/glew.h> // GLuint

class Texture;
class RenderingServer;

class Cubemap {
public:
    static Cubemap from_cubemap(RenderingServer& rs, const std::shared_ptr<Texture>& cubemap_texture);
    static Cubemap from_panorama(RenderingServer& rs, const std::shared_ptr<Texture>& panorama_texture);
    static void static_init();
    static void static_clean_up();

    const Texture& get_texture() {
        return *cubemap_texture;
    }

    void draw();

private:
    static GLuint vertices_id;
    std::shared_ptr<Texture> cubemap_texture;
    RenderingServer& rendering_server;

    Cubemap(RenderingServer& rs, const std::shared_ptr<Texture>& cubemap_texture);
};
