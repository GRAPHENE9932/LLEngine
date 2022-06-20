#pragma once

#include <string>
#include <memory>

#include <GL/glew.h>
#include <glm/mat4x4.hpp>

#include "../common/Texture.hpp"
#include "../structs/Rect.hpp"
#include "SpatialObject.hpp"
#include "DrawableObject.hpp"

class ImageObject2D : public SpatialObject, public DrawableObject {
public:
    static GLuint program_id;

    ImageObject2D(std::shared_ptr<Texture> texture, bool is_transparent);

    static void pre_init();
    static void clean_up();

    void set_screen_space_position(const glm::vec3& scr_space_pos, const glm::vec2 win_size);
    void set_in_center_of_screen(const glm::vec2 win_size, const float z_coord);
    void set_screen_space_scale(const glm::vec3& scr_space_scale, const glm::vec2 win_size);
    void change_texture(std::shared_ptr<Texture> texture);

    void draw(GLfloat* camera_mvp) override;

private:
    static GLuint vertices_id, uvs_id, uvs_inv_v_id, matrix_uniform_id;

    std::shared_ptr<Texture> texture;
    bool is_transparent;
};
