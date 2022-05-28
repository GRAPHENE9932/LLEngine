#pragma once

#include <string>
#include <memory>

#include <GL/glew.h>
#include <glm/mat4x4.hpp>

#include "../structs/Rect.hpp"

class ImageObject2D {
public:
    static GLuint program_id;

    ImageObject2D(std::shared_ptr<utils::ManagedTextureID> texture_id, const Rect& rect, bool is_transparent);

    static void pre_init();
    static void clean_up();

    void change_rect(const Rect& new_rect);
    void change_texture(std::shared_ptr<utils::ManagedTextureID> texture_id);

    void draw() const;
private:
    static GLuint indices_id, vertices_id, uvs_id,
        matrix_uniform_id;

    glm::mat4 matrix;
    std::shared_ptr<utils::ManagedTextureID> texture_id;
    bool is_transparent;
};