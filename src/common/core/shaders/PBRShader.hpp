#pragma once

#include "nodes/core/rendering/PointLightNode.hpp" // PointLightNode
#include "common/core/Material.hpp"
#include "common/core/Skybox.hpp"

#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4

#include <set> // std::set
#include <cstdint> // uint8_t, uint32_t
#include <optional> // std::optional

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
        USING_ENVIRONMENT_CUBEMAP = 0x80000
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

    static Parameters to_parameters(RenderingServer& rs, const Material& material) noexcept;

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
        RenderingServer& rs,
        const Material& material, const glm::mat4& mvp_matrix,
        const glm::mat4& model_matrix, const glm::vec3& camera_position
    ) const;
    void delete_shader();

    Parameters extract_parameters() const noexcept;
    GLuint get_program_id() const noexcept;

    inline bool is_initialized() const noexcept {
        return program_id != 0;
    }
    inline uint32_t get_point_lights_count() const noexcept {
        return point_light_ids.size();
    }

private:
    GLuint program_id = 0;

    Flags flags = NO_FLAGS;
    Channel metallic_channel = Channel::NONE;
    Channel roughness_channel = Channel::NONE;
    Channel ao_channel = Channel::NONE;

    GLint mvp_id = -1;
    GLint model_matrix_id = -1;
    GLint normal_matrix_id = -1;
    GLint ambient_id = -1;
    GLint camera_position_id = -1;
    GLint base_color_factor_id = -1;
    GLint normal_scale_id = -1;
    GLint metallic_factor_id = -1;
    GLint roughness_factor_id = -1;
    GLint ao_factor_id = -1;
    GLint uv_offset_id = -1;
    GLint uv_scale_id = -1;
    GLint base_uv_offset_id = -1;
    GLint base_uv_scale_id = -1;
    GLint normal_uv_offset_id = -1;
    GLint normal_uv_scale_id = -1;
    GLint metallic_uv_offset_id = -1;
    GLint metallic_uv_scale_id = -1;
    GLint roughness_uv_offset_id = -1;
    GLint roughness_uv_scale_id = -1;
    GLint ao_uv_offset_id = -1;
    GLint ao_uv_scale_id = -1;

    GLint base_color_texture_uniform_id = -1;
    GLint normal_map_texture_uniform_id = -1;
    GLint ao_texture_uniform_id = -1;
    GLint metallic_texture_uniform_id = -1;
    GLint roughness_texture_uniform_id = -1;
    GLint emissive_texture_uniform_id = -1;
    GLint environment_cubemap_uniform_id = -1;

    std::set<PointLightNode::Uniforms> point_light_ids;

    void initialize_uniforms(const Parameters& params);
};
