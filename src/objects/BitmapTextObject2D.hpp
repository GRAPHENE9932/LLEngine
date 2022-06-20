#pragma once

#include <memory>
#include <string>
#include <vector>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/gtx/quaternion.hpp>
#include <GL/glew.h>

#include "../common/BitmapFont.hpp"
#include "SpatialObject.hpp"
#include "DrawableObject.hpp"

class BitmapTextObject2D : public SpatialObject, public DrawableObject {
public:
    BitmapTextObject2D(const std::shared_ptr<BitmapFont>& font,
            const std::string& text, const glm::vec3& color,
            const glm::vec2 win_size);
    ~BitmapTextObject2D();

    static void pre_init();
    static void clean_up();
    static GLuint get_program_id();
    
    void set_screen_space_position(const glm::vec3& scr_space_pos, const glm::vec2 win_size);
    void set_screen_space_scale(const glm::vec3& scr_space_scale, const glm::vec2 win_size);
    /// Appearance of the object will be changed only after
    /// call of the set_text function.
    void set_font(const std::shared_ptr<BitmapFont>& font);
    void set_text(const std::string& text);

    void draw(GLfloat* camera_mvp) override;

private:
    static GLuint program_id, model_matrix_uniform_id, color_uniform_id, mvp_matrix_uniform_id;

    GLuint vertices_id = 0, uvs_id = 0, texture_id = 0;

    std::shared_ptr<BitmapFont> font;

    glm::vec3 color;

    std::vector<float> vertices;
    std::vector<float> uvs;

    void register_buffers();
};
