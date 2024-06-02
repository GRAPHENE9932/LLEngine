#pragma once

#include "rendering/Material.hpp"
#include "rendering/Shader.hpp"
#include "datatypes.hpp"

#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4

#include <cstdint> // uint8_t, uint32_t
#include <optional> // std::optional

namespace llengine {
class PBRShader {
public:
    enum Flags : uint32_t {
        NO_FLAGS = 0x0,
        USING_BASE_COLOR_TEXTURE = 0x1,
        USING_BASE_COLOR_FACTOR = 0x2,
        USING_VERTEX_NORMALS = 0x4,
        USING_NORMAL_TEXTURE = 0x8,
        USING_NORMAL_MAP_SCALE = 0x10,
        USING_FRAGMENT_POSITION = 0x20,
        USING_UV = 0x40,
        USING_GENERAL_UV_TRANSFORM = 0x80,
        USING_BASE_UV_TRANSFORM = 0x100,
        USING_NORMAL_UV_TRANSFORM = 0x200,
        USING_METALLIC_UV_TRANSFORM = 0x400,
        USING_ROUGHNESS_UV_TRANSFORM = 0x800,
        USING_AO_UV_TRANSFORM = 0x1000,
        USING_METALLIC_TEXTURE = 0x2000,
        USING_METALLIC_FACTOR = 0x4000,
        USING_ROUGHNESS_TEXTURE = 0x8000,
        USING_ROUGHNESS_FACTOR = 0x10000,
        USING_AO_TEXTURE = 0x20000,
        USING_AO_FACTOR = 0x40000,
        USING_IBL = 0x80000,
        USING_SHADOW_MAP = 0x100000,
        USING_EMISSIVE_TEXTURE = 0x200000,
        USING_EMISSIVE_FACTOR = 0x400000
    };

    friend inline constexpr Flags operator|(Flags left, Flags right) noexcept {
        return static_cast<Flags>(static_cast<uint32_t>(left) | static_cast<uint32_t>(right));
    }
    friend inline constexpr Flags& operator|=(Flags& left, Flags right) noexcept {
        return left = static_cast<Flags>(static_cast<uint32_t>(left) | static_cast<uint32_t>(right));
    }

    struct Parameters {
        size_t point_lights_count;
        Flags flags;
        Channel metallic_channel;
        Channel roughness_channel;
        Channel ao_channel;

        auto operator<=>(const Parameters& other) const noexcept = default;
    };

    static Parameters to_parameters(const Material& material) noexcept;

    explicit PBRShader(const Parameters& params);
    // Make the object non-copyable.
    // It was so frustrating to debug an error caused by
    // an unexpected destructor call...
    PBRShader(const PBRShader&) = delete;
    PBRShader& operator=(const PBRShader&) = delete;
    PBRShader(PBRShader&&) noexcept = default;

    ~PBRShader();

    void initialize(const Parameters& params);
    void use_shader(
        const Material& material, const glm::mat4& mvp_matrix,
        const glm::mat4& model_matrix, const glm::vec3& camera_position
    ) const;
    void delete_shader();

    Parameters extract_parameters() const noexcept;
    ShaderID get_program_id() const noexcept;

    inline bool is_initialized() const noexcept {
        return shader.has_value();
    }

private:
    using ShaderType = Shader<
        "mvp", "model_matrix", "normal_matrix", "ambient", "camera_position",
        "base_color_factor", "normal_map_scale", "metallic_factor", "roughness_factor",
        "ao_factor", "emissive_factor", "uv_offset", "uv_scale",
        "base_uv_offset", "base_uv_scale", "normal_uv_offset", "normal_uv_scale",
        "metallic_uv_offset", "metallic_uv_scale", "roughness_uv_offset",
        "roughness_uv_scale", "ao_uv_offset", "ao_uv_scale", "shadow_view_proj_matrix",
        "shadow_map_bias_at_45_deg", "shadow_light_direction", "base_color_texture",
        "normal_texture", "ao_texture", "metallic_texture", "roughness_texture",
        "emissive_texture", "prefiltered_specular_map", "irradiance_map",
        "brdf_integration_map", "shadow_map", "pcf_sparsity", "point_lights[].position",
        "point_lights[].color"
    >;
    std::optional<ShaderType> shader = std::nullopt;

    Flags flags = NO_FLAGS;
    Channel metallic_channel = Channel::NONE;
    Channel roughness_channel = Channel::NONE;
    Channel ao_channel = Channel::NONE;
    std::int32_t point_lights_count = 0;
};
}