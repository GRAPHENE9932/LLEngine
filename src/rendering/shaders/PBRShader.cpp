#include <cassert> // assert

#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

#include "nodes/rendering/PointLightNode.hpp" // PointLightNode
#include "rendering/Material.hpp"
#include "utils/shader_loader.hpp" // load_shaders
#include "rendering/RenderingServer.hpp" // RenderingServer
#include "PBRShader.hpp" // TexturedShared

using namespace llengine;

constexpr std::string_view VERTEX_SHADER_TEXT = 
    #include "shaders/pbr_shader.vert"
;
constexpr std::string_view FRAGMENT_SHADER_TEXT =
    #include "shaders/pbr_shader.frag"
;

PBRShader::Flags compute_flags(RenderingServer& rs, const Material& material) {
    PBRShader::Flags flags = PBRShader::NO_FLAGS;

    if (material.base_color_texture.has_value()) {
        flags |= PBRShader::USING_BASE_COLOR_TEXTURE;
    }
    if (material.base_color_factor != glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)) {
        flags |= PBRShader::USING_BASE_COLOR_FACTOR;
    }
    flags |= PBRShader::USING_VERTEX_NORMALS;
    if (material.normal_map.has_value()) {
        flags |= PBRShader::USING_NORMAL_TEXTURE;
        if (material.normal_map->scale != 1.0f) {
            flags |= PBRShader::USING_NORMAL_MAP_SCALE;
        }
    }
    if (material.ambient_occlusion_texture.has_value()) {
        flags |= PBRShader::USING_AO_TEXTURE;
    }
    if (material.ambient_occlusion_factor != 1.0f) {
        flags |= PBRShader::USING_AO_FACTOR;
    }
    if (material.metallic_texture.has_value()) {
        flags |= PBRShader::USING_METALLIC_TEXTURE;
    }
    if (material.metallic_factor != 1.0f) {
        flags |= PBRShader::USING_METALLIC_FACTOR;
    }
    if (material.roughness_texture.has_value()) {
        flags |= PBRShader::USING_ROUGHNESS_TEXTURE;
    }
    if (material.roughness_factor != 1.0f) {
        flags |= PBRShader::USING_ROUGHNESS_FACTOR;
    }
    if (rs.have_environment_cubemap()) {
        flags |= PBRShader::USING_IBL;
    }
    if (!rs.get_point_lights().empty() || flags & PBRShader::USING_IBL) {
        flags |= PBRShader::USING_FRAGMENT_POSITION;
    }
    if ((flags & PBRShader::USING_BASE_COLOR_TEXTURE) ||
        (flags & PBRShader::USING_NORMAL_TEXTURE) ||
        (flags & PBRShader::USING_AO_TEXTURE) ||
        (flags & PBRShader::USING_METALLIC_TEXTURE) ||
        (flags & PBRShader::USING_ROUGHNESS_TEXTURE)) {
        flags |= PBRShader::USING_UV;
    }
    if (material.has_offsets_and_scales()) {
        if (material.has_identical_offsets_and_scales()) {
            flags |= PBRShader::USING_GENERAL_UV_TRANSFORM;
        }
        else {
            flags |= PBRShader::USING_BASE_UV_TRANSFORM;
            flags |= PBRShader::USING_NORMAL_UV_TRANSFORM;
            flags |= PBRShader::USING_METALLIC_UV_TRANSFORM;
            flags |= PBRShader::USING_ROUGHNESS_UV_TRANSFORM;
            flags |= PBRShader::USING_AO_UV_TRANSFORM;
        }
    }
    if (rs.shadow_mapping_enabled()) {
        flags |= PBRShader::USING_SHADOW_MAP;
    }

    return flags;
}

PBRShader::Parameters
PBRShader::to_parameters(RenderingServer& rs, const Material& material) noexcept {
    Parameters result {
        static_cast<uint32_t>(rs.get_point_lights().size()),
        compute_flags(rs, material)
    };

    result.metallic_channel = material.metallic_texture.has_value() ?
        material.metallic_texture->channel : Channel::NONE;
    result.roughness_channel = material.roughness_texture.has_value() ?
        material.roughness_texture->channel : Channel::NONE;
    result.ao_channel = material.ambient_occlusion_texture.has_value() ?
        material.ambient_occlusion_texture->channel : Channel::NONE;

    return result;
}

PBRShader::PBRShader(const Parameters& params) {
    initialize(params);
}

PBRShader::~PBRShader() {
    delete_shader();
}

void PBRShader::initialize_uniforms(const Parameters& params) {
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
    metallic_uv_offset_id = glGetUniformLocation(program_id, "metallic_uv_offset");
    metallic_uv_scale_id = glGetUniformLocation(program_id, "metallic_uv_scale");
    roughness_uv_offset_id = glGetUniformLocation(program_id, "roughness_uv_offset");
    roughness_uv_scale_id = glGetUniformLocation(program_id, "roughness_uv_scale");
    ao_uv_offset_id = glGetUniformLocation(program_id, "ao_uv_offset");
    ao_uv_scale_id = glGetUniformLocation(program_id, "ao_uv_scale");
    dir_light_view_proj_matrix_id = glGetUniformLocation(program_id, "dir_light_view_proj_matrix");
    shadow_map_bias_id = glGetUniformLocation(program_id, "shadow_map_bias");

    base_color_texture_uniform_id = glGetUniformLocation(program_id, "base_color_texture");
    normal_map_texture_uniform_id = glGetUniformLocation(program_id, "normal_texture");
    ao_texture_uniform_id = glGetUniformLocation(program_id, "ao_texture");
    metallic_texture_uniform_id = glGetUniformLocation(program_id, "metallic_texture");
    roughness_texture_uniform_id = glGetUniformLocation(program_id, "roughness_texture");
    emissive_texture_uniform_id = glGetUniformLocation(program_id, "emmisive_texture");
    prefiltered_specular_map_uniform_id = glGetUniformLocation(program_id, "prefiltered_specular_map");
    irradiance_map_uniform_id = glGetUniformLocation(program_id, "irradiance_map");
    brdf_integration_map_uniform_id = glGetUniformLocation(program_id, "brdf_integration_map");
    shadow_map_uniform_id = glGetUniformLocation(program_id, "shadow_map");

    for (size_t i = 0; i < params.point_lights_count; i++) {
        point_light_ids.insert(
            PointLightNode::get_uniforms_id(program_id, "point_lights", i)
        );
    }
}

void PBRShader::initialize(const Parameters& params) {
    using std::string_literals::operator""s;

    flags = params.flags;
    metallic_channel = params.metallic_channel;
    roughness_channel = params.roughness_channel;
    ao_channel = params.ao_channel;

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
    if (flags & USING_METALLIC_UV_TRANSFORM)
        defines.emplace_back("USING_METALLIC_UV_TRANSFORM");
    if (flags & USING_ROUGHNESS_UV_TRANSFORM)
        defines.emplace_back("USING_ROUGHNESS_UV_TRANSFORM");
    if (flags & USING_AO_UV_TRANSFORM)
        defines.emplace_back("USING_AO_UV_TRANSFORM");
    if (flags & USING_METALLIC_TEXTURE) {
        defines.emplace_back("USING_METALLIC_TEXTURE");
        defines.emplace_back("METALLIC_TEXTURE_CHANNEL "s + channel_to_char(metallic_channel));
    }
    if (flags & USING_METALLIC_FACTOR)
        defines.emplace_back("USING_METALLIC_FACTOR");
    if (flags & USING_ROUGHNESS_TEXTURE) {
        defines.emplace_back("USING_ROUGHNESS_TEXTURE");
        defines.emplace_back("ROUGHNESS_TEXTURE_CHANNEL "s + channel_to_char(roughness_channel));
    }
    if (flags & USING_ROUGHNESS_FACTOR)
        defines.emplace_back("USING_ROUGHNESS_FACTOR");
    if (flags & USING_AO_TEXTURE) {
        defines.emplace_back("USING_AO_TEXTURE");
        defines.emplace_back("AO_TEXTURE_CHANNEL "s + channel_to_char(ao_channel));
    }
    if (flags & USING_AO_FACTOR)
        defines.emplace_back("USING_AO_FACTOR");
    if (flags & USING_IBL)
        defines.emplace_back("USING_IBL");
    if (flags & USING_SHADOW_MAP) {
        defines.emplace_back("USING_SHADOW_MAP");
    }

    program_id = load_shaders(
        VERTEX_SHADER_TEXT,
        FRAGMENT_SHADER_TEXT,
        defines
    );

    initialize_uniforms(params);
}

void PBRShader::use_shader(
    RenderingServer& rs,
    const Material& material, const glm::mat4& mvp_matrix,
    const glm::mat4& model_matrix, const glm::vec3& camera_position
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
    glUniform3fv(ambient_id, 1, glm::value_ptr(glm::vec3(0.01f, 0.01f, 0.01f)));
    glUniform3fv(camera_position_id, 1, glm::value_ptr(camera_position));
    glUniform4fv(base_color_factor_id, 1, glm::value_ptr(material.base_color_factor));
    if (material.normal_map.has_value()) {
        glUniform1fv(normal_scale_id, 1, &material.normal_map->scale);
    }
    glUniform1f(metallic_factor_id, material.metallic_factor);
    glUniform1f(roughness_factor_id, material.roughness_factor);
    glUniform1f(ao_factor_id, material.ambient_occlusion_factor);
    glUniformMatrix4fv(dir_light_view_proj_matrix_id, 1, GL_FALSE, glm::value_ptr(rs.get_dir_light_view_proj_matrix()));
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
    if ((metallic_uv_offset_id != -1 || metallic_uv_scale_id != -1) && material.metallic_texture.has_value()) {
        glUniform2fv(metallic_uv_offset_id, 1, glm::value_ptr(material.metallic_texture->uv_offset));
        glUniform2fv(metallic_uv_scale_id, 1, glm::value_ptr(material.metallic_texture->uv_scale));
    }
    if ((roughness_uv_offset_id != -1 || roughness_uv_scale_id != -1) && material.roughness_texture.has_value()) {
        glUniform2fv(roughness_uv_offset_id, 1, glm::value_ptr(material.roughness_texture->uv_offset));
        glUniform2fv(roughness_uv_scale_id, 1, glm::value_ptr(material.roughness_texture->uv_scale));
    }
    if ((ao_uv_offset_id != -1 || ao_uv_scale_id != -1) && material.ambient_occlusion_texture.has_value()) {
        glUniform2fv(ao_uv_offset_id, 1, glm::value_ptr(material.ambient_occlusion_texture->uv_offset));
        glUniform2fv(ao_uv_scale_id, 1, glm::value_ptr(material.ambient_occlusion_texture->uv_scale));
    }
    if (shadow_map_bias_id != -1) {
        glUniform1f(shadow_map_bias_id, rs.get_shadow_map_bias());
    }
    auto point_light_ids_iter = point_light_ids.begin();
    for (auto& cur_point_light : rs.get_point_lights()) {
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
    if (ao_texture_uniform_id != -1) {
        glUniform1i(ao_texture_uniform_id, cur_tex_unit);
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
    if (prefiltered_specular_map_uniform_id != -1) {
        glUniform1i(prefiltered_specular_map_uniform_id, cur_tex_unit);
        glActiveTexture(GL_TEXTURE0 + cur_tex_unit);
        glBindTexture(GL_TEXTURE_CUBE_MAP, rs.get_prefiltered_specular_map(camera_position).value().get().get_id());
        cur_tex_unit++;
    }
    if (irradiance_map_uniform_id != -1) {
        glUniform1i(irradiance_map_uniform_id, cur_tex_unit);
        glActiveTexture(GL_TEXTURE0 + cur_tex_unit);
        glBindTexture(GL_TEXTURE_CUBE_MAP, rs.get_irradiance_map(camera_position).value().get().get_id());
        cur_tex_unit++;
    }
    if (brdf_integration_map_uniform_id != -1) {
        glUniform1i(brdf_integration_map_uniform_id, cur_tex_unit);
        glActiveTexture(GL_TEXTURE0 + cur_tex_unit);
        glBindTexture(GL_TEXTURE_2D, rs.get_brdf_integration_map().get_id());
        cur_tex_unit++;
    }
    if (shadow_map_uniform_id != -1) {
        glUniform1i(shadow_map_uniform_id, cur_tex_unit);
        glActiveTexture(GL_TEXTURE0 + cur_tex_unit);
        glBindTexture(GL_TEXTURE_2D, rs.get_shadow_map_texture_id());
        cur_tex_unit++;
    }
}

void PBRShader::delete_shader() {
    if (is_initialized()) {
        glDeleteProgram(program_id);
        program_id = 0;
        point_light_ids.clear();
    }
}

PBRShader::Parameters PBRShader::extract_parameters() const noexcept {
    return {
        static_cast<uint32_t>(point_light_ids.size()),
        flags,
        metallic_channel,
        roughness_channel,
        ao_channel
    };
}

GLuint PBRShader::get_program_id() const noexcept {
    return program_id;
}
