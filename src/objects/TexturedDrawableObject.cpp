#include "TexturedDrawableObject.hpp"
#include "../utils/shader_loader.hpp"

GLuint TexturedDrawableObject::program_id = 0;
GLuint TexturedDrawableObject::mvp_matrix_uniform_id = 0;
GLuint TexturedDrawableObject::model_matrix_uniform_id = 0;
GLuint TexturedDrawableObject::normal_matrix_uniform_id = 0;
GLuint TexturedDrawableObject::light_position_uniform_id = 0;
std::array<SpotLight::Uniforms, SPOT_LIGHTS_AMOUNT> TexturedDrawableObject::spot_light_uniforms;
std::array<PointLight::Uniforms, POINT_LIGHTS_AMOUNT> TexturedDrawableObject::point_light_uniforms;

TexturedDrawableObject::TexturedDrawableObject(std::shared_ptr<Texture> texture,
    std::shared_ptr<Mesh> mesh) :
    texture(texture) {
    this->mesh = mesh;
}

void TexturedDrawableObject::pre_init() {
    // Init shaders.
    if constexpr (POINT_LIGHTS_AMOUNT != 0) {
        program_id = load_shaders(
            "res/shaders/textured_vertex.glsl",
            "res/shaders/textured_fragment.glsl"
        );
    }
    else {
        program_id = load_shaders(
            "res/shaders/textured_no_point_lights_vertex.glsl",
            "res/shaders/textured_no_point_lights_fragment.glsl"
        );
    }
    // Init uniforms.
    mvp_matrix_uniform_id = glGetUniformLocation(program_id, "MVP");
    model_matrix_uniform_id = glGetUniformLocation(program_id, "MODEL_MATRIX");
    if constexpr (POINT_LIGHTS_AMOUNT != 0)
        normal_matrix_uniform_id = glGetUniformLocation(program_id, "NORMAL_MATRIX");
    light_position_uniform_id = glGetUniformLocation(program_id, "LIGHT_POSITION");

    for (GLuint i = 0; i < SPOT_LIGHTS_AMOUNT; i++)
        spot_light_uniforms[i] = SpotLight::get_uniforms_id(program_id, "SPOT_LIGHTS", i);

    if constexpr (POINT_LIGHTS_AMOUNT != 0) {
        for (GLuint i = 0; i < POINT_LIGHTS_AMOUNT; i++)
            point_light_uniforms[i] = PointLight::get_uniforms_id(program_id, "POINT_LIGHTS", i);
    }
}

void TexturedDrawableObject::clean_up() {
    program_id = 0;
    mvp_matrix_uniform_id = 0;
    model_matrix_uniform_id = 0;
    normal_matrix_uniform_id = 0;
    light_position_uniform_id = 0;

    glDeleteProgram(program_id);
}

void TexturedDrawableObject::draw(DrawParameters& params) {
    if (program_id == 0)
        pre_init();
    
    if (params.cur_shader != program_id) {
        params.cur_shader = program_id;
        glUseProgram(program_id);
    }

    // Vertices.
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->get_vertices_id());
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // UVs.
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->get_uvs_id());
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

    // Normals.
    if constexpr (POINT_LIGHTS_AMOUNT != 0) {
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, mesh->get_normals_id());
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
    }

    // Bind textures.
    glBindTexture(GL_TEXTURE_2D, texture->get_id());

    // Uniforms.
    const glm::mat4 model_matrix = compute_matrix();

    const glm::mat4 mvp = params.view_proj_matrix * model_matrix;
    glUniformMatrix4fv(mvp_matrix_uniform_id, 1, GL_FALSE, &mvp[0][0]);

    glUniformMatrix4fv(model_matrix_uniform_id, 1, GL_FALSE, &model_matrix[0][0]);

    if constexpr (POINT_LIGHTS_AMOUNT != 0) {
        glm::mat4 normal_matrix = glm::transpose(glm::inverse(model_matrix));
        glUniformMatrix4fv(normal_matrix_uniform_id, 1, GL_FALSE, &normal_matrix[0][0]);

        for (GLuint i = 0; i < POINT_LIGHTS_AMOUNT; i++)
            params.point_lights[i].set_uniforms(point_light_uniforms[i], params.overlay_mode);
    }

    for (GLuint i = 0; i < SPOT_LIGHTS_AMOUNT; i++)
        params.spot_lights[i].set_uniforms(spot_light_uniforms[i], params.overlay_mode);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->get_indices_id());
    glDrawElements(GL_TRIANGLES, mesh->get_indices().size(), GL_UNSIGNED_SHORT, 0);
    params.triangles_drawn += mesh->get_indices().size() / 3;

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}
