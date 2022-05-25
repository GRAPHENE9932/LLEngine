#pragma once

#include "../utils/utils.hpp"
#include "DrawableObject.hpp"

class TexturedDrawableObject : public DrawableObject {
public:
    static GLuint program_id;

    std::shared_ptr<utils::ManagedTextureID> texture_id;

    TexturedDrawableObject(std::shared_ptr<utils::ManagedTextureID> texture_id,
        std::shared_ptr<Mesh> mesh);
    ~TexturedDrawableObject();

    static void pre_init();
    static void clean_up();

    void draw(GLfloat* camera_mvp, GLfloat* view_matrix, GLfloat* light_direction);

private:
    static GLuint mvp_matrix_uniform_id, object_matrix_uniform_id,
        camera_matrix_uniform_id, light_direction_uniform_id;
};
