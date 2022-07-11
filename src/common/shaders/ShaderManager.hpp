#pragma once

#include <vector>
#include <memory>

#include "ColoredTextShader.hpp"
#include "SkyboxShader.hpp"
#include "TexturedShader.hpp"
#include "UnshadedShader.hpp"
#include "UnshadedTexturedShader.hpp"

class ShaderManager {
public:
    void use_colored_text_shader(const glm::mat4& mvp, const glm::vec3& color);
    void use_skybox_shader(const glm::mat4& mvp);
    void use_textured_shader(const glm::mat4& mvp, const glm::mat4& model_matrix,
                             const bool overlay,
                             std::vector<std::shared_ptr<SpotLight>>& spot_lights,
                             std::vector<std::shared_ptr<PointLight>>& point_lights);
    void use_unshaded_shader(const glm::mat4& mvp, const glm::vec3& color);
    void use_unshaded_textured_shader(const glm::mat4& mvp);

    GLuint get_colored_text_program_id();
    GLuint get_skybox_program_id();
    GLuint get_textured_program_id(std::size_t spot_lights_count,
                                   std::size_t point_lights_count);
    GLuint get_unshaded_program_id();
    GLuint get_unshaded_textured_program_id();

    /// Initialize all shaders, except ones that have dynamic macros,
    /// like TexturedShader.
    void initialize_all();

private:
    ColoredTextShader colored_text_shader;
    SkyboxShader skybox_shader;
    std::vector<TexturedShader> textured_shaders;
    UnshadedShader unshaded_shader;
    UnshadedTexturedShader unshaded_textured_shader;
};
