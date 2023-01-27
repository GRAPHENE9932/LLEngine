#include <cassert> // assert

#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

#include "nodes/core/rendering/PointLightNode.hpp" // PointLightNode
#include "utils/shader_loader.hpp" // load_shaders
#include "RenderingServer.hpp" // RenderingServer
#include "CommonShader.hpp" // TexturedShared

CommonShader::Flags compute_flags(const Material& material, RenderingServer& rs) {
    CommonShader::Flags flags = CommonShader::NO_FLAGS;

    if (material.base_color.texture.has_value())
        flags |= CommonShader::USING_BASE_COLOR_TEXTURE;
    if (material.base_color.factor != glm::vec4(1.0f, 1.0f, 1.0f, 1.0f))
        flags |= CommonShader::USING_BASE_COLOR_FACTOR;
    if (!rs.get_point_lights().empty()) {
        flags |= CommonShader::USING_VERTEX_NORMALS;
        if (material.normal_map.has_value()) {
            flags |= CommonShader::USING_NORMAL_TEXTURE;
            if (material.normal_map->scale != 1.0f)
                flags |= CommonShader::USING_NORMAL_MAP_SCALE;
        }
    }
    if (rs.get_point_lights().size() > 0)
        flags |= CommonShader::USING_FRAGMENT_POSITION;
    if ((flags & CommonShader::USING_BASE_COLOR_TEXTURE) ||
        (flags & CommonShader::USING_NORMAL_TEXTURE))
        flags |= CommonShader::USING_UV;
    if (material.have_offsets_and_scales()) {
        if (material.have_identical_offsets_and_scales()) {
            flags |= CommonShader::USING_GENERAL_UV_TRANSFORM;
        }
        else {
            flags |= CommonShader::USING_BASE_UV_TRANSFORM;
            flags |= CommonShader::USING_NORMAL_UV_TRANSFORM;
        }
    }

    return flags;
}

CommonShader::Parameters
CommonShader::to_parameters(const Material& material, RenderingServer& rs) noexcept {
    return {
        compute_flags(material, rs),
        static_cast<uint32_t>(rs.get_point_lights().size())
    };
}

CommonShader::CommonShader(const Parameters& params, RenderingServer& rs) :
    rendering_server(rs) {
    initialize(params);
}

CommonShader::~CommonShader() {
    delete_shader();
}

void CommonShader::initialize(const Parameters& params) {
    flags = params.flags;

    // Declare #defines for the shader.
    std::vector<std::string> defines {
        "POINT_LIGHTS_COUNT " + std::to_string(params.point_lights_count)
    };

    if (flags & USING_BASE_COLOR_TEXTURE)
        defines.push_back("USING_BASE_COLOR_TEXTURE");
    if (flags & USING_BASE_COLOR_FACTOR)
        defines.push_back("USING_BASE_COLOR_FACTOR");
    if (flags & USING_VERTEX_NORMALS)
        defines.push_back("USING_VERTEX_NORMALS");
    if (flags & USING_NORMAL_TEXTURE)
        defines.push_back("USING_NORMAL_TEXTURE");
    if (flags & USING_NORMAL_MAP_SCALE)
        defines.push_back("USING_NORMAL_MAP_SCALE");
    if (flags & USING_FRAGMENT_POSITION)
        defines.push_back("USING_FRAGMENT_POSITION");
    if (flags & USING_UV)
        defines.push_back("USING_UV");
    if (flags & USING_GENERAL_UV_TRANSFORM)
        defines.push_back("USING_GENERAL_UV_TRANSFORM");
    if (flags & USING_BASE_UV_TRANSFORM)
        defines.push_back("USING_BASE_UV_TRANSFORM");
    if (flags & USING_NORMAL_UV_TRANSFORM)
        defines.push_back("USING_NORMAL_UV_TRANSFORM");

    program_id = load_shaders(
        "res/shaders/textured.vert",
        "res/shaders/textured.frag",
        defines
    );

    // Initialize uniform locations.
    mvp_id = glGetUniformLocation(program_id, "MVP");
    model_matrix_id = glGetUniformLocation(program_id, "MODEL_MATRIX");
    normal_matrix_id = glGetUniformLocation(program_id, "NORMAL_MATRIX");
    ambient_id = glGetUniformLocation(program_id, "AMBIENT");
    base_color_factor_id = glGetUniformLocation(program_id, "BASE_COLOR_FACTOR");
    normal_scale_id = glGetUniformLocation(program_id, "NORMAL_MAP_SCALE");
    uv_offset_id = glGetUniformLocation(program_id, "UV_OFFSET");
    uv_scale_id = glGetUniformLocation(program_id, "UV_SCALE");
    base_uv_offset_id = glGetUniformLocation(program_id, "BASE_UV_OFFSET");
    base_uv_scale_id = glGetUniformLocation(program_id, "BASE_UV_SCALE");
    normal_uv_offset_id = glGetUniformLocation(program_id, "NORMAL_UV_OFFSET");
    normal_uv_scale_id = glGetUniformLocation(program_id, "NORMAL_UV_SCALE");

    base_color_texture_uniform_id = glGetUniformLocation(program_id, "BASE_COLOR_TEXTURE");
    normal_map_texture_uniform_id = glGetUniformLocation(program_id, "NORMAL_TEXTURE");
    met_rough_texture_uniform_id = glGetUniformLocation(program_id, "METALLIC_ROUGHNESS_TEXTURE");
    occlusion_texture_uniform_id = glGetUniformLocation(program_id, "OCCLUSION_TEXTURE");
    emmisive_texture_uniform_id = glGetUniformLocation(program_id, "EMMISIVE_TEXTURE");
    
    for (size_t i = 0; i < params.point_lights_count; i++) {
        point_light_ids.insert(
            PointLightNode::get_uniforms_id(program_id, "POINT_LIGHTS", i)
        );
    }
}

void CommonShader::use_shader(const Material& material, const glm::mat4& mvp_matrix,
    const glm::mat4& model_matrix) const {
    assert(is_initialized());

    glUseProgram(program_id);

    // Set uniforms.
    glUniformMatrix4fv(mvp_id, 1, GL_FALSE, glm::value_ptr(mvp_matrix));
    glUniformMatrix4fv(model_matrix_id, 1, GL_FALSE, glm::value_ptr(model_matrix));
    if (normal_matrix_id != -1) {
        glm::mat4 normal_matrix = glm::transpose(glm::inverse(
            glm::mat3(model_matrix)
        ));
        glUniformMatrix4fv(normal_matrix_id, 1, GL_FALSE, glm::value_ptr(normal_matrix));
    }
    glUniform3fv(ambient_id, 1, glm::value_ptr(glm::vec3(0.2f, 0.2f, 0.2f)));
    glUniform4fv(base_color_factor_id, 1, glm::value_ptr(material.base_color.factor));
    if (material.normal_map.has_value()) {
        glUniform1fv(normal_scale_id, 1, &material.normal_map->scale);
    }
    if (uv_offset_id != -1 || uv_scale_id != -1) {
        const std::pair<glm::vec2, glm::vec2> general_offset_scale =
                material.get_general_uv_offset_and_scale();
        glUniform2fv(uv_offset_id, 1, glm::value_ptr(general_offset_scale.first));
        glUniform2fv(uv_scale_id, 1, glm::value_ptr(general_offset_scale.second));
    }
    if ((base_uv_offset_id != -1 || base_uv_scale_id != -1) && material.base_color.texture.has_value()) {
        glUniform2fv(base_uv_offset_id, 1, glm::value_ptr(material.base_color.texture->uv_offset));
        glUniform2fv(base_uv_scale_id, 1, glm::value_ptr(material.base_color.texture->uv_scale));
    }
    if ((normal_uv_offset_id != -1 || normal_uv_scale_id != -1) && material.normal_map.has_value()) {
        glUniform2fv(normal_uv_offset_id, 1, glm::value_ptr(material.normal_map->texture.uv_offset));
        glUniform2fv(normal_uv_scale_id, 1, glm::value_ptr(material.normal_map->texture.uv_scale));
    }
    auto point_light_ids_iter = point_light_ids.begin();
    for (auto& cur_point_light : rendering_server.get_point_lights()) {
        cur_point_light->set_uniforms(*point_light_ids_iter);
        point_light_ids_iter++;
    }

    GLenum cur_tex_unit = 0;
    if (base_color_texture_uniform_id != -1) {
        glUniform1i(base_color_texture_uniform_id, cur_tex_unit);
        glActiveTexture(GL_TEXTURE0 + cur_tex_unit);
        glBindTexture(GL_TEXTURE_2D, material.base_color.texture.value().texture->get_id());
        cur_tex_unit++;
    }
    if (normal_map_texture_uniform_id != -1) {
        glUniform1i(normal_map_texture_uniform_id, cur_tex_unit);
        glActiveTexture(GL_TEXTURE0 + cur_tex_unit);
        glBindTexture(GL_TEXTURE_2D, material.normal_map.value().texture.texture->get_id());
        cur_tex_unit++;
    }
    if (met_rough_texture_uniform_id != -1) {
        glUniform1i(met_rough_texture_uniform_id, cur_tex_unit);
        glActiveTexture(GL_TEXTURE0 + cur_tex_unit);
        glBindTexture(GL_TEXTURE_2D, material.metallic_roughness.texture.value().texture->get_id());
        cur_tex_unit++;
    }
    if (occlusion_texture_uniform_id != -1) {
        glUniform1i(occlusion_texture_uniform_id, cur_tex_unit);
        glActiveTexture(GL_TEXTURE0 + cur_tex_unit);
        glBindTexture(GL_TEXTURE_2D, material.occlusion.texture.value().texture->get_id());
        cur_tex_unit++;
    }
    if (emmisive_texture_uniform_id != -1) {
        glUniform1i(emmisive_texture_uniform_id, cur_tex_unit);
        glActiveTexture(GL_TEXTURE0 + cur_tex_unit);
        glBindTexture(GL_TEXTURE_2D, material.emmisive.texture.value().texture->get_id());
        cur_tex_unit++;
    }
}

void CommonShader::delete_shader() {
    if (is_initialized()) {
        glDeleteProgram(program_id);
        program_id = 0;
        point_light_ids.clear();
    }
}

CommonShader::Parameters CommonShader::extract_parameters() const noexcept {
    return {
        flags,
        static_cast<uint32_t>(point_light_ids.size())
    };
}

GLuint CommonShader::get_program_id() const noexcept {
    return program_id;
}
