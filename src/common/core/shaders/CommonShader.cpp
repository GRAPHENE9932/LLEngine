#include <cassert> // assert

#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

#include "nodes/core/rendering/PointLightNode.hpp" // PointLightNode
#include "common/core/Material.hpp"
#include "utils/shader_loader.hpp" // load_shaders
#include "RenderingServer.hpp" // RenderingServer
#include "CommonShader.hpp" // TexturedShared

CommonShader::Flags compute_flags(
    const Material& material, bool using_environment_cubemap
) {
    CommonShader::Flags flags = CommonShader::NO_FLAGS;

    if (material.base_color_texture.has_value()) {
        flags |= CommonShader::USING_BASE_COLOR_TEXTURE;
    }
    if (material.base_color_factor != glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)) {
        flags |= CommonShader::USING_BASE_COLOR_FACTOR;
    }
    if (!RenderingServer::get_instance().get_point_lights().empty()) {
        flags |= CommonShader::USING_VERTEX_NORMALS;
        if (material.normal_map.has_value()) {
            flags |= CommonShader::USING_NORMAL_TEXTURE;
            if (material.normal_map->scale != 1.0f) {
                flags |= CommonShader::USING_NORMAL_MAP_SCALE;
            }
        }
    }
    if (material.ambient_occlusion_texture.has_value()) {
        flags |= CommonShader::USING_AMBIENT_OCCLUSION_TEXTURE;
    }
    if (material.ambient_occlusion_factor != 1.0f) {
        flags |= CommonShader::USING_AMBIENT_OCCLUSION_FACTOR;
    }
    if (material.metallic_texture.has_value()) {
        flags |= CommonShader::USING_METALLIC_TEXTURE;
    }
    if (material.metallic_factor != 1.0f) {
        flags |= CommonShader::USING_METALLIC_FACTOR;
    }
    if (material.roughness_texture.has_value()) {
        flags |= CommonShader::USING_ROUGHNESS_TEXTURE;
    }
    if (material.roughness_factor != 1.0f) {
        flags |= CommonShader::USING_ROUGHNESS_FACTOR;
    }
    if (using_environment_cubemap) {
        flags |= CommonShader::USING_ENVIRONMENT_CUBEMAP;
    }
    if (!RenderingServer::get_instance().get_point_lights().empty() || flags & CommonShader::USING_ENVIRONMENT_CUBEMAP) {
        flags |= CommonShader::USING_FRAGMENT_POSITION;
    }
    if ((flags & CommonShader::USING_BASE_COLOR_TEXTURE) ||
        (flags & CommonShader::USING_NORMAL_TEXTURE) ||
        (flags & CommonShader::USING_AMBIENT_OCCLUSION_TEXTURE) ||
        (flags & CommonShader::USING_METALLIC_TEXTURE) ||
        (flags & CommonShader::USING_ROUGHNESS_TEXTURE)) {
        flags |= CommonShader::USING_UV;
    }
    if (material.has_offsets_and_scales()) {
        if (material.has_identical_offsets_and_scales()) {
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
CommonShader::to_parameters(
    const Material& material, bool using_environment_cubemap
) noexcept {
    return {
        compute_flags(material, using_environment_cubemap),
        static_cast<uint32_t>(RenderingServer::get_instance().get_point_lights().size())
    };
}

CommonShader::CommonShader(const Parameters& params) {
    initialize(params);
}

CommonShader::~CommonShader() {
    delete_shader();
}

void CommonShader::initialize_uniforms(const Parameters& params) {
    mvp_id = glGetUniformLocation(program_id, "mvp");
    model_matrix_id = glGetUniformLocation(program_id, "model_matrix");
    normal_matrix_id = glGetUniformLocation(program_id, "normal_matrix");
    ambient_id = glGetUniformLocation(program_id, "ambient");
    camera_position_id = glGetUniformLocation(program_id, "camera_position");
    base_color_factor_id = glGetUniformLocation(program_id, "base_color_factor");
    normal_scale_id = glGetUniformLocation(program_id, "normal_map_scale");
    uv_offset_id = glGetUniformLocation(program_id, "uv_offset");
    uv_scale_id = glGetUniformLocation(program_id, "uv_scale");
    base_uv_offset_id = glGetUniformLocation(program_id, "base_uv_offset");
    base_uv_scale_id = glGetUniformLocation(program_id, "base_uv_scale");
    normal_uv_offset_id = glGetUniformLocation(program_id, "normal_uv_offset");
    normal_uv_scale_id = glGetUniformLocation(program_id, "normal_uv_scale");

    base_color_texture_uniform_id = glGetUniformLocation(program_id, "base_color_texture");
    normal_map_texture_uniform_id = glGetUniformLocation(program_id, "normal_texture");
    ambient_occlusion_texture_uniform_id = glGetUniformLocation(program_id, "ambient_occlusion_texture");
    metallic_texture_uniform_id = glGetUniformLocation(program_id, "metallic_texture");
    roughness_texture_uniform_id = glGetUniformLocation(program_id, "roughness_texture");
    emissive_texture_uniform_id = glGetUniformLocation(program_id, "emmisive_texture");
    environment_cubemap_uniform_id = glGetUniformLocation(program_id, "environment_cubemap");

    for (size_t i = 0; i < params.point_lights_count; i++) {
        point_light_ids.insert(
            PointLightNode::get_uniforms_id(program_id, "point_lights", i)
        );
    }
}

void CommonShader::initialize(const Parameters& params) {
    flags = params.flags;

    // Declare #defines for the shader.
    std::vector<std::string> defines {
        "POINT_LIGHTS_COUNT " + std::to_string(params.point_lights_count)
    };

    if (flags & USING_BASE_COLOR_TEXTURE)
        defines.emplace_back("USING_BASE_COLOR_TEXTURE");
    if (flags & USING_BASE_COLOR_FACTOR)
        defines.emplace_back("USING_BASE_COLOR_FACTOR");
    if (flags & USING_VERTEX_NORMALS)
        defines.emplace_back("USING_VERTEX_NORMALS");
    if (flags & USING_NORMAL_TEXTURE)
        defines.emplace_back("USING_NORMAL_TEXTURE");
    if (flags & USING_NORMAL_MAP_SCALE)
        defines.emplace_back("USING_NORMAL_MAP_SCALE");
    if (flags & USING_FRAGMENT_POSITION)
        defines.emplace_back("USING_FRAGMENT_POSITION");
    if (flags & USING_UV)
        defines.emplace_back("USING_UV");
    if (flags & USING_GENERAL_UV_TRANSFORM)
        defines.emplace_back("USING_GENERAL_UV_TRANSFORM");
    if (flags & USING_BASE_UV_TRANSFORM)
        defines.emplace_back("USING_BASE_UV_TRANSFORM");
    if (flags & USING_NORMAL_UV_TRANSFORM)
        defines.emplace_back("USING_NORMAL_UV_TRANSFORM");
    if (flags & USING_METALLIC_TEXTURE)
        defines.emplace_back("USING_METALLIC_TEXTURE");
    if (flags & USING_METALLIC_FACTOR)
        defines.emplace_back("USING_METALLIC_FACTOR");
    if (flags & USING_ROUGHNESS_TEXTURE)
        defines.emplace_back("USING_ROUGHNESS_TEXTURE");
    if (flags & USING_ROUGHNESS_FACTOR)
        defines.emplace_back("USING_ROUGHNESS_FACTOR");
    if (flags & USING_AMBIENT_OCCLUSION_TEXTURE)
        defines.emplace_back("USING_AMBIENT_OCCLUSION_TEXTURE");
    if (flags & USING_AMBIENT_OCCLUSION_FACTOR)
        defines.emplace_back("USING_AMBIENT_OCCLUSION_FACTOR");
    if (flags & USING_ENVIRONMENT_CUBEMAP)
        defines.emplace_back("USING_ENVIRONMENT_CUBEMAP");

    program_id = load_shaders(
        "res/shaders/textured.vert",
        "res/shaders/textured.frag",
        defines
    );

    initialize_uniforms(params);
}

void CommonShader::use_shader(
    const Material& material, const glm::mat4& mvp_matrix,
    const glm::mat4& model_matrix, const glm::vec3& camera_position,
    std::optional<std::reference_wrapper<const Texture>> environment_cubemap
) const {
    assert(is_initialized());

    glUseProgram(program_id);

    glUniformMatrix4fv(mvp_id, 1, GL_FALSE, glm::value_ptr(mvp_matrix));
    glUniformMatrix4fv(model_matrix_id, 1, GL_FALSE, glm::value_ptr(model_matrix));
    if (normal_matrix_id != -1) {
        glm::mat4 normal_matrix = glm::transpose(glm::inverse(
            glm::mat3(model_matrix)
        ));
        glUniformMatrix4fv(normal_matrix_id, 1, GL_FALSE, glm::value_ptr(normal_matrix));
    }
    glUniform3fv(ambient_id, 1, glm::value_ptr(glm::vec3(0.2f, 0.2f, 0.2f)));
    glUniform3fv(camera_position_id, 1, glm::value_ptr(camera_position));
    glUniform4fv(base_color_factor_id, 1, glm::value_ptr(material.base_color_factor));
    if (material.normal_map.has_value()) {
        glUniform1fv(normal_scale_id, 1, &material.normal_map->scale);
    }
    if (uv_offset_id != -1 || uv_scale_id != -1) {
        const std::pair<glm::vec2, glm::vec2> general_offset_scale =
                material.get_general_uv_offset_and_scale();
        glUniform2fv(uv_offset_id, 1, glm::value_ptr(general_offset_scale.first));
        glUniform2fv(uv_scale_id, 1, glm::value_ptr(general_offset_scale.second));
    }
    if ((base_uv_offset_id != -1 || base_uv_scale_id != -1) && material.base_color_texture.has_value()) {
        glUniform2fv(base_uv_offset_id, 1, glm::value_ptr(material.base_color_texture->uv_offset));
        glUniform2fv(base_uv_scale_id, 1, glm::value_ptr(material.base_color_texture->uv_scale));
    }
    if ((normal_uv_offset_id != -1 || normal_uv_scale_id != -1) && material.normal_map.has_value()) {
        glUniform2fv(normal_uv_offset_id, 1, glm::value_ptr(material.normal_map->texture.uv_offset));
        glUniform2fv(normal_uv_scale_id, 1, glm::value_ptr(material.normal_map->texture.uv_scale));
    }
    auto point_light_ids_iter = point_light_ids.begin();
    for (auto& cur_point_light : RenderingServer::get_instance().get_point_lights()) {
        cur_point_light->set_uniforms(*point_light_ids_iter);
        point_light_ids_iter++;
    }
    GLint cur_tex_unit = 0;
    if (base_color_texture_uniform_id != -1) {
        glUniform1i(base_color_texture_uniform_id, cur_tex_unit);
        glActiveTexture(GL_TEXTURE0 + cur_tex_unit);
        glBindTexture(GL_TEXTURE_2D, material.base_color_texture.value().texture->get_id());
        cur_tex_unit++;
    }
    if (normal_map_texture_uniform_id != -1) {
        glUniform1i(normal_map_texture_uniform_id, cur_tex_unit);
        glActiveTexture(GL_TEXTURE0 + cur_tex_unit);
        glBindTexture(GL_TEXTURE_2D, material.normal_map.value().texture.texture->get_id());
        cur_tex_unit++;
    }
    if (ambient_occlusion_texture_uniform_id != -1) {
        glUniform1i(ambient_occlusion_texture_uniform_id, cur_tex_unit);
        glActiveTexture(GL_TEXTURE0 + cur_tex_unit);
        glBindTexture(GL_TEXTURE_2D, material.ambient_occlusion_texture.value().texture->get_id());
        cur_tex_unit++;
    }
    if (metallic_texture_uniform_id != -1) {
        glUniform1i(metallic_texture_uniform_id, cur_tex_unit);
        glActiveTexture(GL_TEXTURE0 + cur_tex_unit);
        glBindTexture(GL_TEXTURE_2D, material.metallic_texture.value().texture->get_id());
        cur_tex_unit++;
    }
    if (roughness_texture_uniform_id != -1) {
        glUniform1i(roughness_texture_uniform_id, cur_tex_unit);
        glActiveTexture(GL_TEXTURE0 + cur_tex_unit);
        glBindTexture(GL_TEXTURE_2D, material.roughness_texture.value().texture->get_id());
        cur_tex_unit++;
    }
    if (emissive_texture_uniform_id != -1) {
        glUniform1i(emissive_texture_uniform_id, cur_tex_unit);
        glActiveTexture(GL_TEXTURE0 + cur_tex_unit);
        glBindTexture(GL_TEXTURE_2D, material.emissive_texture.value().texture->get_id());
        cur_tex_unit++;
    }
    if (environment_cubemap_uniform_id != -1) {
        glUniform1i(environment_cubemap_uniform_id, cur_tex_unit);
        glActiveTexture(GL_TEXTURE0 + cur_tex_unit);
        glBindTexture(GL_TEXTURE_CUBE_MAP, environment_cubemap.value().get().get_id());
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
