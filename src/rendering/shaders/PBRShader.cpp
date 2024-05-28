#include <cassert> // assert

#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

#include "nodes/rendering/PointLightNode.hpp" // PointLightNode
#include "rendering/Material.hpp"
#include "utils/shader_loader.hpp" // load_shaders
#include "rendering/RenderingServer.hpp" // RenderingServer
#include "PBRShader.hpp" // TexturedShared

using namespace llengine;

constexpr std::string_view VERTEX_SHADER_TEXT =
    #include "shaders/objects/pbr/pbr_shader.vert"
;
constexpr std::string_view FRAGMENT_SHADER_TEXT =
    #include "shaders/objects/pbr/pbr_shader.frag"
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
    if (material.emissive_texture.has_value()) {
        flags |= PBRShader::USING_EMISSIVE_TEXTURE;
    }
    if (material.emissive_factor != glm::vec3(0.0f, 0.0f, 0.0f)) {
        flags |= PBRShader::USING_EMISSIVE_FACTOR;
    }
    if (rs.has_environment_cubemap()) {
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
    if (rs.is_shadow_mapping_enabled()) {
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

[[nodiscard]] static std::vector<std::string> compute_defines_from_params(const PBRShader::Parameters& params) {
    using namespace std::string_literals;

    std::vector<std::string> defines {
        "POINT_LIGHTS_COUNT " + std::to_string(params.point_lights_count)
    };

    const auto& flags = params.flags;
    if (flags & PBRShader::USING_BASE_COLOR_TEXTURE)
        defines.emplace_back("USING_BASE_COLOR_TEXTURE");
    if (flags & PBRShader::USING_BASE_COLOR_FACTOR)
        defines.emplace_back("USING_BASE_COLOR_FACTOR");
    if (flags & PBRShader::USING_VERTEX_NORMALS)
        defines.emplace_back("USING_VERTEX_NORMALS");
    if (flags & PBRShader::USING_NORMAL_TEXTURE)
        defines.emplace_back("USING_NORMAL_TEXTURE");
    if (flags & PBRShader::USING_NORMAL_MAP_SCALE)
        defines.emplace_back("USING_NORMAL_MAP_SCALE");
    if (flags & PBRShader::USING_FRAGMENT_POSITION)
        defines.emplace_back("USING_FRAGMENT_POSITION");
    if (flags & PBRShader::USING_UV)
        defines.emplace_back("USING_UV");
    if (flags & PBRShader::USING_GENERAL_UV_TRANSFORM)
        defines.emplace_back("USING_GENERAL_UV_TRANSFORM");
    if (flags & PBRShader::USING_BASE_UV_TRANSFORM)
        defines.emplace_back("USING_BASE_UV_TRANSFORM");
    if (flags & PBRShader::USING_NORMAL_UV_TRANSFORM)
        defines.emplace_back("USING_NORMAL_UV_TRANSFORM");
    if (flags & PBRShader::USING_METALLIC_UV_TRANSFORM)
        defines.emplace_back("USING_METALLIC_UV_TRANSFORM");
    if (flags & PBRShader::USING_ROUGHNESS_UV_TRANSFORM)
        defines.emplace_back("USING_ROUGHNESS_UV_TRANSFORM");
    if (flags & PBRShader::USING_AO_UV_TRANSFORM)
        defines.emplace_back("USING_AO_UV_TRANSFORM");
    if (flags & PBRShader::USING_METALLIC_TEXTURE) {
        defines.emplace_back("USING_METALLIC_TEXTURE");
        defines.emplace_back("METALLIC_TEXTURE_CHANNEL "s + channel_to_char(params.metallic_channel));
    }
    if (flags & PBRShader::USING_METALLIC_FACTOR)
        defines.emplace_back("USING_METALLIC_FACTOR");
    if (flags & PBRShader::USING_ROUGHNESS_TEXTURE) {
        defines.emplace_back("USING_ROUGHNESS_TEXTURE");
        defines.emplace_back("ROUGHNESS_TEXTURE_CHANNEL "s + channel_to_char(params.roughness_channel));
    }
    if (flags & PBRShader::USING_ROUGHNESS_FACTOR)
        defines.emplace_back("USING_ROUGHNESS_FACTOR");
    if (flags & PBRShader::USING_AO_TEXTURE) {
        defines.emplace_back("USING_AO_TEXTURE");
        defines.emplace_back("AO_TEXTURE_CHANNEL "s + channel_to_char(params.ao_channel));
    }
    if (flags & PBRShader::USING_AO_FACTOR)
        defines.emplace_back("USING_AO_FACTOR");
    if (flags & PBRShader::USING_IBL)
        defines.emplace_back("USING_IBL");
    if (flags & PBRShader::USING_SHADOW_MAP) {
        defines.emplace_back("USING_SHADOW_MAP");
    }
    if (flags & PBRShader::USING_EMISSIVE_TEXTURE) {
        defines.emplace_back("USING_EMISSIVE_TEXTURE");
    }
    if (flags & PBRShader::USING_EMISSIVE_FACTOR) {
        defines.emplace_back("USING_EMISSIVE_FACTOR");
    }

    return defines;
}

void PBRShader::initialize(const Parameters& params) {
    flags = params.flags;
    metallic_channel = params.metallic_channel;
    roughness_channel = params.roughness_channel;
    ao_channel = params.ao_channel;
    point_lights_count = static_cast<std::int32_t>(params.point_lights_count);

    shader = std::make_optional<ShaderType>(
        VERTEX_SHADER_TEXT, FRAGMENT_SHADER_TEXT,
        compute_defines_from_params(params)
    );
}

void PBRShader::use_shader(
    RenderingServer& rs,
    const Material& material, const glm::mat4& mvp_matrix,
    const glm::mat4& model_matrix, const glm::vec3& camera_position
) const {
    assert(is_initialized());
    assert(shader.has_value());

    shader->use_shader();
    shader->set_mat4<"mvp">(mvp_matrix);
    shader->set_mat4<"model_matrix">(model_matrix);
    if (shader->is_uniform_initialized<"normal_matrix">()) {
        glm::mat4 normal_matrix = glm::transpose(glm::inverse(
            glm::mat3(model_matrix)
        ));
        shader->set_mat4<"normal_matrix">(normal_matrix);
    }
    shader->set_vec3<"ambient">(glm::vec3(0.01f, 0.01f, 0.01f));
    shader->set_vec3<"camera_position">(camera_position);
    shader->set_vec4<"base_color_factor">(material.base_color_factor);
    if (material.normal_map.has_value()) {
        shader->set_float<"normal_map_scale">(material.normal_map->scale);
    }
    shader->set_float<"metallic_factor">(material.metallic_factor);
    shader->set_float<"roughness_factor">(material.roughness_factor);
    shader->set_vec3<"emissive_factor">(material.emissive_factor);
    shader->set_float<"ao_factor">(material.ambient_occlusion_factor);
    shader->set_mat4<"shadow_view_proj_matrix">(rs.get_shadow_map().get_view_proj_matrix());
    if (shader->is_uniform_initialized<"uv_offset">() || shader->is_uniform_initialized<"uv_scale">()) {
        const std::pair<glm::vec2, glm::vec2> general_offset_scale =
                material.get_general_uv_offset_and_scale();
        shader->set_vec2<"uv_offset">(general_offset_scale.first);
        shader->set_vec2<"uv_scale">(general_offset_scale.second);
    }
    if (
        (shader->is_uniform_initialized<"base_uv_offset">() || shader->is_uniform_initialized<"base_uv_scale">()) &&
        material.base_color_texture.has_value()
    ) {
        shader->set_vec2<"base_uv_offset">(material.base_color_texture->uv_offset);
        shader->set_vec2<"base_uv_scale">(material.base_color_texture->uv_scale);
    }
    if (
        (shader->is_uniform_initialized<"normal_uv_offset">() || shader->is_uniform_initialized<"normal_uv_scale">()) &&
        material.normal_map.has_value()
    ) {
        shader->set_vec2<"normal_uv_offset">(material.normal_map->texture.uv_offset);
        shader->set_vec2<"normal_uv_scale">(material.normal_map->texture.uv_scale);
    }
    if (
        (shader->is_uniform_initialized<"metallic_uv_offset">() || shader->is_uniform_initialized<"metallic_uv_scale">()) &&
        material.metallic_texture.has_value()
    ) {
        shader->set_vec2<"metallic_uv_offset">(material.metallic_texture->uv_offset);
        shader->set_vec2<"metallic_uv_scale">(material.metallic_texture->uv_scale);
    }
    if (
        (shader->is_uniform_initialized<"roughness_uv_offset">() || shader->is_uniform_initialized<"roughness_uv_scale">()) &&
        material.roughness_texture.has_value()
    ) {
        shader->set_vec2<"roughness_uv_offset">(material.roughness_texture->uv_offset);
        shader->set_vec2<"roughness_uv_scale">(material.roughness_texture->uv_scale);
    }
    if (
        (shader->is_uniform_initialized<"ao_uv_offset">() || shader->is_uniform_initialized<"ao_uv_scale">()) &&
        material.roughness_texture.has_value()
    ) {
        shader->set_vec2<"ao_uv_offset">(material.ambient_occlusion_texture->uv_offset);
        shader->set_vec2<"ao_uv_scale">(material.ambient_occlusion_texture->uv_scale);
    }
    if (shader->is_uniform_initialized<"shadow_map_bias_at_45_deg">()) {
        shader->set_float<"shadow_map_bias_at_45_deg">(rs.get_shadow_map().get_adjusted_bias_at_45_deg());
    }
    if (shader->is_uniform_initialized<"pcf_sparsity">()) {
        assert(rs.get_shadow_map().get_size().x == rs.get_shadow_map().get_size().y);
        shader->set_float<"pcf_sparsity">(1.0f / rs.get_shadow_map().get_size().x);
    }
    if (shader->is_uniform_initialized<"shadow_light_direction">()) {
        shader->set_vec3<"shadow_light_direction">(rs.get_shadow_map().get_light_direction());
    }
    const auto& point_lights = rs.get_point_lights();
    for (std::size_t i = 0; i < point_lights.size(); i++) {
        shader->set_vec3<"point_lights[].position">(i, point_lights[i]->get_global_position());
        shader->set_vec3<"point_lights[].color">(i, point_lights[i]->color);
    }

    GraphicsAPIEnum texture_unit {0};
    if (shader->is_uniform_initialized<"base_color_texture">()) {
        shader->bind_2d_texture<"base_color_texture">(material.base_color_texture.value().texture->get_id(), texture_unit++);
    }
    if (shader->is_uniform_initialized<"normal_texture">()) {
        shader->bind_2d_texture<"normal_texture">(material.normal_map.value().texture.texture->get_id(), texture_unit++);
    }
    if (shader->is_uniform_initialized<"ao_texture">()) {
        shader->bind_2d_texture<"ao_texture">(material.ambient_occlusion_texture.value().texture->get_id(), texture_unit++);
    }
    if (shader->is_uniform_initialized<"metallic_texture">()) {
        shader->bind_2d_texture<"metallic_texture">(material.metallic_texture.value().texture->get_id(), texture_unit++);
    }
    if (shader->is_uniform_initialized<"roughness_texture">()) {
        shader->bind_2d_texture<"roughness_texture">(material.roughness_texture.value().texture->get_id(), texture_unit++);
    }
    if (shader->is_uniform_initialized<"emissive_texture">()) {
        shader->bind_2d_texture<"emissive_texture">(material.emissive_texture.value().texture->get_id(), texture_unit++);
    }
    if (shader->is_uniform_initialized<"prefiltered_specular_map">()) {
        auto prefiltered_specular_map_id = rs.get_prefiltered_specular_map(camera_position).value().get().get_id();
        shader->use_shader();
        shader->bind_cubemap_texture<"prefiltered_specular_map">(prefiltered_specular_map_id, texture_unit++);
    }
    if (shader->is_uniform_initialized<"irradiance_map">()) {
        auto irradiance_map_id = rs.get_irradiance_map(camera_position).value().get().get_id();
        shader->use_shader();
        shader->bind_cubemap_texture<"irradiance_map">(irradiance_map_id, texture_unit++);
    }
    if (shader->is_uniform_initialized<"brdf_integration_map">()) {
        auto brdf_integration_map_id = rs.get_brdf_integration_map().get_id();
        shader->use_shader();
        shader->bind_2d_texture<"brdf_integration_map">(brdf_integration_map_id, texture_unit++);
    }
    if (shader->is_uniform_initialized<"shadow_map">()) {
        shader->bind_2d_texture<"shadow_map">(rs.get_shadow_map().get_texture_id(), texture_unit++);
    }
}

void PBRShader::delete_shader() {
    shader = std::nullopt;
}

PBRShader::Parameters PBRShader::extract_parameters() const noexcept {
    return {
        static_cast<uint32_t>(point_lights_count),
        flags,
        metallic_channel,
        roughness_channel,
        ao_channel
    };
}

GLuint PBRShader::get_program_id() const noexcept {
    return shader->get_program_id();
}
