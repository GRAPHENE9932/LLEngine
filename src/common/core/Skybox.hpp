#pragma once

#include <memory> // std::shared_ptr

#include <GL/glew.h> // GLuint

class Texture;
class RenderingServer;

class Skybox {
public:
    std::shared_ptr<Texture> texture;

    Skybox(RenderingServer& rs, const std::shared_ptr<Texture>& texture);

    static void static_init();
    static void static_clean_up();

    void draw();

private:
    RenderingServer& rendering_server;

    static GLuint vertices_id;
};
