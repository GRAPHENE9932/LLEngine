#pragma once

#include <string>
#include <memory>

#include <GL/glew.h>
#include <glm/mat4x4.hpp>

#include "../common/Texture.hpp"
#include "../structs/Rect.hpp"
#include "SpatialObject.hpp"
#include "DrawableObject.hpp"

class ImageObject : public SpatialObject, public DrawableObject {
public:
    static GLuint program_id;

    ImageObject(std::shared_ptr<Texture> texture, bool is_transparent, bool is_2d = true);

    static void pre_init();
    static void clean_up();

    void set_screen_space_position(const glm::vec3& scr_space_pos, const glm::vec2 win_size);
    void set_in_center_of_screen(const glm::vec2 win_size, const float z_coord);
    void set_screen_space_scale(const glm::vec3& scr_space_scale, const glm::vec2 win_size);
    void change_texture(std::shared_ptr<Texture> texture);

    void draw(DrawParameters& params) override;
    GLuint get_program_id() const override {
        return program_id;
    }

private:
    static GLuint vertices_id, uvs_id, uvs_inv_v_id, model_matrix_uniform_id, mvp_matrix_uniform_id;

    std::shared_ptr<Texture> texture;
    bool is_transparent, is_2d;
};
