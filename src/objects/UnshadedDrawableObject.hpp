#pragma once

#include "DrawableObject.hpp"

class UnshadedDrawableObject : public DrawableObject {
public:
    static GLuint program_id;

    UnshadedDrawableObject(std::shared_ptr<Mesh> mesh, glm::vec3 color);
    ~UnshadedDrawableObject();

    static void pre_init();
    static void clean_up();

    glm::vec3 color;

    void draw(GLfloat* camera_mvp);

private:
    static GLuint mvp_matrix_uniform_id, model_matrix_uniform_id, color_uniform_id;
};
