#include "TexturedDrawableObject.hpp"

GLuint TexturedDrawableObject::program_id;
GLuint TexturedDrawableObject::mvp_matrix_uniform_id;
GLuint TexturedDrawableObject::model_matrix_uniform_id;
GLuint TexturedDrawableObject::normal_matrix_uniform_id;
GLuint TexturedDrawableObject::camera_direction_uniform_id;
GLuint TexturedDrawableObject::light_position_uniform_id;
std::array<PointLight::Uniforms, TX_DRW_POINT_LIGHTS_AMOUNT> TexturedDrawableObject::point_light_uniforms;

TexturedDrawableObject::TexturedDrawableObject(std::shared_ptr<utils::ManagedTextureID> texture_id,
    std::shared_ptr<Mesh> mesh) :
    texture_id(texture_id) {
    this->mesh = mesh;
}

TexturedDrawableObject::~TexturedDrawableObject() {

}

void TexturedDrawableObject::pre_init() {
    // Init shaders.
    program_id = utils::load_shaders("res/shaders/textured_vertex.glsl",
                                     "res/shaders/textured_fragment.glsl");
    // Init uniforms.
    mvp_matrix_uniform_id = glGetUniformLocation(program_id, "MVP");
    model_matrix_uniform_id = glGetUniformLocation(program_id, "MODEL_MATRIX");
    normal_matrix_uniform_id = glGetUniformLocation(program_id, "NORMAL_MATRIX");
    camera_direction_uniform_id = glGetUniformLocation(program_id, "CAMERA_DIRECTION");
    light_position_uniform_id = glGetUniformLocation(program_id, "LIGHT_POSITION");

    for (GLuint i = 0; i < TX_DRW_POINT_LIGHTS_AMOUNT; i++)
        point_light_uniforms[i] = PointLight::get_uniforms_id(program_id, "POINT_LIGHTS", i);
}

void TexturedDrawableObject::clean_up() {
    glDeleteProgram(program_id);
}

void TexturedDrawableObject::draw(GLfloat* camera_mvp, GLfloat* camera_dir,
                                  const std::array<PointLight, TX_DRW_POINT_LIGHTS_AMOUNT>& point_lights) {
    // Vertices.
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vertices_id);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // UVs.
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->uvs_id);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

    // Normals.
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->normals_id);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // Bind textures.
    glBindTexture(GL_TEXTURE_2D, texture_id->texture_id);

    // Uniforms.
    glUniformMatrix4fv(mvp_matrix_uniform_id, 1, GL_FALSE, camera_mvp);
    auto model_matrix = compute_matrix();
    glUniformMatrix4fv(model_matrix_uniform_id, 1, GL_FALSE, &model_matrix[0][0]);
    auto normal_matrix = glm::transpose(glm::inverse(model_matrix));
    glUniformMatrix4fv(normal_matrix_uniform_id, 1, GL_FALSE, &normal_matrix[0][0]);
    glUniform3fv(camera_direction_uniform_id, 1, &camera_dir[0]);

    for (GLuint i = 0; i < TX_DRW_POINT_LIGHTS_AMOUNT; i++)
        point_lights[i].set_uniforms(point_light_uniforms[i]);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indices_id);
    glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_SHORT, 0);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}
