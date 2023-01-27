#pragma once

#include "common/core/Material.hpp" // Material
#include "nodes/core/rendering/PointLightNode.hpp" // PointLightNode

#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4

#include <set> // std::set
#include <cstdint> // uint8_t, uint32_t
#include <optional> // std::optional

class RenderingServer;

class CommonShader {
public:
    enum Flags : uint16_t {
        NO_FLAGS = 0x0000,
        USING_BASE_COLOR_TEXTURE = 0x0001,
        USING_BASE_COLOR_FACTOR = 0x0002,
        USING_VERTEX_NORMALS = 0x0004,
        USING_NORMAL_TEXTURE = 0x0008,
        USING_NORMAL_MAP_SCALE = 0x0010,
        USING_FRAGMENT_POSITION = 0x0020,
        USING_UV = 0x0040,
        USING_GENERAL_UV_TRANSFORM = 0x0080,
        USING_BASE_UV_TRANSFORM = 0x0100,
        USING_NORMAL_UV_TRANSFORM = 0x0200
    };

    friend inline constexpr Flags operator|(Flags left, Flags right) noexcept {
        return static_cast<Flags>(static_cast<uint16_t>(left) | static_cast<uint16_t>(right));
    }
    friend inline constexpr Flags& operator|=(Flags& left, Flags right) noexcept {
        return left = static_cast<Flags>(static_cast<uint16_t>(left) | static_cast<uint16_t>(right));
    }

    struct Parameters {
        Flags flags;
        size_t point_lights_count;

        auto operator<=>(const Parameters& other) const noexcept = default;
    };

    static Parameters to_parameters(const Material& material, RenderingServer& rs) noexcept;

    explicit CommonShader(const Parameters& params, RenderingServer& rs);
    // Make the object non-copyable.
    // It was so frustrating to debug an error caused by
    // an unexpected destructor call...
    CommonShader(const CommonShader&) = delete;
    CommonShader& operator=(const CommonShader&) = delete;
    CommonShader(CommonShader&&) noexcept = default;

    ~CommonShader();

    void initialize(const Parameters& params);
    void use_shader(const Material& material, const glm::mat4& mvp_matrix,
        const glm::mat4& model_matrix) const;
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

    GLint mvp_id = -1;
    GLint model_matrix_id = -1;
    GLint normal_matrix_id = -1;
    GLint ambient_id = -1;
    GLint base_color_factor_id = -1;
    GLint normal_scale_id = -1;
    GLint uv_offset_id = -1;
    GLint uv_scale_id = -1;
    GLint base_uv_offset_id = -1;
    GLint base_uv_scale_id = -1;
    GLint normal_uv_offset_id = -1;
    GLint normal_uv_scale_id = -1;

    GLint base_color_texture_uniform_id = -1;
    GLint normal_map_texture_uniform_id = -1;
    GLint met_rough_texture_uniform_id = -1;
    GLint occlusion_texture_uniform_id = -1;
    GLint emmisive_texture_uniform_id = -1;

    std::set<PointLightNode::Uniforms> point_light_ids;

    Flags flags = NO_FLAGS;

    RenderingServer& rendering_server;
};
