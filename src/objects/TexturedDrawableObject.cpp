#include "TexturedDrawableObject.hpp"
#include "utils/shader_loader.hpp"

TexturedDrawableObject::TexturedDrawableObject(std::shared_ptr<Texture> texture,
    std::shared_ptr<Mesh> mesh) :
    texture(texture) {
    this->mesh = mesh;
}

void TexturedDrawableObject::draw(DrawParameters& params) {
    // Uniforms.
    const glm::mat4 model_matrix = compute_matrix();
    const glm::mat4 mvp = params.view_proj_matrix * model_matrix;
    params.sh_mgr.use_textured_shader(mvp, model_matrix, params.overlay_mode,
                                      params.spot_lights, params.point_lights);

    // Vertices.
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->get_vertices_id());
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // UVs.
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->get_uvs_id());
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

    // Normals.
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->get_normals_id());
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // Bind textures.
    glBindTexture(GL_TEXTURE_2D, texture->get_id());
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->get_indices_id());

    glDrawElements(GL_TRIANGLES, mesh->get_indices().size(), GL_UNSIGNED_SHORT, 0);
    params.triangles_drawn += mesh->get_indices().size() / 3;

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}

GLuint TexturedDrawableObject::get_program_id(DrawParameters& params) const {
    return params.sh_mgr.get_textured_program_id(
        params.spot_lights.size(), params.point_lights.size()
    );
}
