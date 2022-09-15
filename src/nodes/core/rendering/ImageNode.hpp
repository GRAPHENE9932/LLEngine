/*#pragma once

#include <memory> // std::shared_ptr

#include <GL/glew.h> // GLuint

#include "common/Texture.hpp" // Texture
#include "DrawableNode.hpp" // DrawableNode

class ImageNode : public DrawableNode {
public:
    ImageNode(const SpatialParams& params, SceneTree& scene_tree,
              std::shared_ptr<Texture> texture,
              bool is_transparent);

    static void static_init();
    static void static_clean_up();

    void set_screen_space_position(const glm::vec3& scr_space_pos, const glm::vec2 win_size);
    void set_in_center_of_screen(const glm::vec2 win_size, const float z_coord);
    void set_screen_space_scale(const glm::vec3& scr_space_scale, const glm::vec2 win_size);
    void change_texture(std::shared_ptr<Texture> texture);

    void update() override;
    GLuint get_program_id() const override;

private:
    static GLuint vertices_id, uvs_id;

    std::shared_ptr<Texture> texture;
    bool is_transparent;
};
*/