/*#include <array>

#include <glm/gtx/transform.hpp>

#include "utils/shader_loader.hpp"
#include "consts_and_enums.hpp"
#include "utils/math.hpp"
#include "SceneTree.hpp" // SceneTree
#include "ImageNode.hpp"

GLuint ImageNode::vertices_id = 0;
GLuint ImageNode::uvs_id = 0;

ImageNode::ImageNode(const SpatialParams& params, SceneTree& scene_tree,
                     std::shared_ptr<Texture> texture,
                     bool is_transparent) :
                     DrawableNode(params, scene_tree),
                     is_transparent(is_transparent) {
    change_texture(texture);
}

void ImageNode::change_texture(std::shared_ptr<Texture> texture) {
    this->texture = texture;
}

void ImageNode::static_init() {
    // Init buffers.
    glGenBuffers(1, &vertices_id);
    glBindBuffer(GL_ARRAY_BUFFER, vertices_id);
    glBufferData(GL_ARRAY_BUFFER, QUAD_VERTICES.size() * sizeof(float),
                 QUAD_VERTICES.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &uvs_id);
    glBindBuffer(GL_ARRAY_BUFFER, uvs_id);
    glBufferData(GL_ARRAY_BUFFER, QUAD_UVS.size() * sizeof(float),
                 QUAD_UVS.data(), GL_STATIC_DRAW);
}

void ImageNode::static_clean_up() {
    vertices_id = 0;
    uvs_id = 0;

    glDeleteBuffers(1, &vertices_id);
    glDeleteBuffers(1, &uvs_id);

}

void ImageNode::set_screen_space_position(const glm::vec3& scr_space_pos, const glm::vec2 win_size) {
    set_translation(glm::vec3(
        utils::scr_space_pos_to_gl_space(scr_space_pos, win_size),
        scr_space_pos.z
    ));
}

void ImageNode::set_in_center_of_screen(const glm::vec2 win_size, const float z_coord) {
    set_translation(glm::vec3(
        -static_cast<glm::vec2>(texture->get_size()) / win_size,
        z_coord
    ));
}

void ImageNode::set_screen_space_scale(const glm::vec3& scr_space_scale, const glm::vec2 win_size) {
    set_scale(glm::vec3(
        utils::scr_space_scale_to_gl_space(
            static_cast<glm::vec2>(scr_space_scale) * static_cast<glm::vec2>(texture->get_size()),
            win_size
        ),
        scr_space_scale.z
    ));
}


void ImageNode::update() {
    update_children();
    const auto& context = scene_tree.get_context();

    if (is_transparent)
        glEnable(GL_BLEND);

    // Uniforms.
    glm::mat4 model_matrix = get_global_matrix();
    glm::mat4 mvp = context.view_proj_matrix * model_matrix;
    scene_tree.get_shader_manager().use_unshaded_textured_shader(mvp);

    // Vertices.
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertices_id);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // UVs.
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, uvs_id);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

    // Bind textures.
    glBindTexture(GL_TEXTURE_2D, texture->get_id());

    glBindBuffer(GL_ARRAY_BUFFER, vertices_id);
    glDrawArrays(GL_TRIANGLES, 0, QUAD_VERTICES.size());
    scene_tree.report_about_drawn_triangles(QUAD_VERTICES.size() / 3);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    if (is_transparent)
        glDisable(GL_BLEND);
}

GLuint ImageNode::get_program_id() const {
    return scene_tree.get_shader_manager().get_unshaded_textured_program_id();
}
*/