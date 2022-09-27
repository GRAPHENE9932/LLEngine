#pragma once

#include <vector> // std::vector
#include <memory> // std::shared_ptr
#include <string_view> // std::string_view

#include <GL/glew.h> // GLuint
#include <glm/vec2.hpp> // glm::vec2
#include <glm/vec3.hpp> // glm::vec3

#include "DrawableNode.hpp" // DrawableNode
#include "common/core/BitmapFont.hpp" // BitmapFont

class BitmapTextNode : public DrawableNode {
public:
    BitmapTextNode(const SpatialParams& params, SceneTree& scene_tree,
                   const std::shared_ptr<BitmapFont>& font,
                   std::string_view text, const glm::vec3& color);
    ~BitmapTextNode();
    
    /// Appearance of the object will be changed only after
    /// call of the set_text function.
    void set_font(const std::shared_ptr<BitmapFont>& font);
    void set_text(std::string_view text);
    void set_screen_space_position(const glm::vec3& scr_space_pos, const glm::vec2& win_size);
    void set_screen_space_scale(const glm::vec3& scr_space_scale, const glm::vec2& win_size);

    void update() override;
    GLuint get_program_id() const override;

private:
    GLuint vertices_id = 0, uvs_id = 0, texture_id = 0;

    std::shared_ptr<BitmapFont> font;

    glm::vec3 color;

    std::vector<float> vertices;
    std::vector<float> uvs;

    void register_buffers();
};
