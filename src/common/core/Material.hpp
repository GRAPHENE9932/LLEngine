#pragma once

#include <array> // std::array
#include <memory> // std::shared_ptr
#include <optional> // std::optional

#include <glm/vec2.hpp> // glm::vec2
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <utility>

#include "Texture.hpp" // Texture

template<typename TEX_TYPE>
struct BasicMaterial {
    struct TextureInfo {
        TEX_TYPE texture;
        glm::vec2 uv_offset = {0.0f, 0.0f};
        glm::vec2 uv_scale = {1.0f, 1.0f};
    };

    struct {
        std::optional<TextureInfo> texture = std::nullopt;
        glm::vec4 factor = {1.0f, 1.0f, 1.0f, 1.0f};
    } base_color;
    struct {
        std::optional<TextureInfo> texture = std::nullopt;
        float metallic_factor = 1.0f;
        float roughness_factor = 1.0f;
    } metallic_roughness;
    struct NormalMap {
        TextureInfo texture;
        float scale = 1.0f;
    };
    std::optional<NormalMap> normal_map = std::nullopt;
    struct {
        std::optional<TextureInfo> texture = std::nullopt;
        float strength = 1.0f;
    } occlusion;
    struct {
        std::optional<TextureInfo> texture = std::nullopt;
        glm::vec3 factor = {0.0f, 0.0f, 0.0f};
    } emmisive;

    [[nodiscard]] bool have_offsets_and_scales() const noexcept {
        return {
            have_offsets_and_scales(base_color) ||
            have_offsets_and_scales(metallic_roughness) ||
            have_offsets_and_scales(normal_map) ||
            have_offsets_and_scales(occlusion) ||
            have_offsets_and_scales(emmisive)
        };
    }

    [[nodiscard]] bool have_identical_offsets_and_scales() const noexcept {
        std::array<std::optional<std::pair<glm::vec2, glm::vec2>>, 5> array {
            base_color.texture.has_value() ?
            std::make_optional(std::make_pair(base_color.texture->uv_offset, base_color.texture->uv_scale)) :
            std::nullopt,

            metallic_roughness.texture.has_value() ?
            std::make_optional(std::make_pair(metallic_roughness.texture->uv_offset, metallic_roughness.texture->uv_scale)) :
            std::nullopt,

            normal_map.has_value() ?
            std::make_optional(std::make_pair(normal_map->texture.uv_offset, normal_map->texture.uv_scale)) :
            std::nullopt,

            occlusion.texture.has_value() ?
            std::make_optional(std::make_pair(occlusion.texture->uv_offset, occlusion.texture->uv_scale)) :
            std::nullopt,

            emmisive.texture.has_value() ?
            std::make_optional(std::make_pair(emmisive.texture->uv_offset, emmisive.texture->uv_scale)) :
            std::nullopt
        };

        // Check if all elements are equal (don't check nullopt objects).
        std::optional<std::pair<glm::vec2, glm::vec2>> previous = std::nullopt;
        for (size_t i = 0; i < array.size(); i++) {
            const auto& current = array[i];

            if (current.has_value()) {
                if (previous.has_value() && *previous != *current)
                    return false;

                previous = current;
            }
        }

        return true;
    }

    /// The material must have identical UV offsets and scales, otherwise
    /// result is undefined.
    /// @returns Pair of two vectors: offset and scale.
    [[nodiscard]] std::pair<glm::vec2, glm::vec2> get_general_uv_offset_and_scale() const noexcept {
        assert(have_identical_offsets_and_scales());
        // In this function we assume that all uv_offsets and uv_scales are identical
        // between base color texture, metallic-roughness texture, normal map, etc.
        // So, just return any of them.
        if (base_color.texture.has_value())
            return {base_color.texture->uv_offset, base_color.texture->uv_scale};
        if (metallic_roughness.texture.has_value())
            return {metallic_roughness.texture->uv_offset, metallic_roughness.texture->uv_scale};
        if (normal_map.has_value())
            return {normal_map->texture.uv_offset, normal_map->texture.uv_scale};
        if (occlusion.texture.has_value())
            return {occlusion.texture->uv_offset, occlusion.texture->uv_scale};
        if (emmisive.texture.has_value())
            return {emmisive.texture->uv_offset, emmisive.texture->uv_scale};
        return {{0.0f, 0.0f}, {1.0f, 1.0f}};
    }

private:
    template<typename T>
    bool have_offsets_and_scales(const T& struct_with_tex) const noexcept {
        return {
            struct_with_tex.texture.has_value() &&
            (struct_with_tex.texture->uv_offset != glm::vec2(0.0f, 0.0f) ||
            struct_with_tex.texture->uv_scale != glm::vec2(1.0f, 1.0f))
        };
    }
    bool have_offsets_and_scales(const std::optional<NormalMap>& normal_map) const noexcept {
        return {
            normal_map.has_value() &&
            (normal_map->texture.uv_offset != glm::vec2(0.0f, 0.0f) ||
            normal_map->texture.uv_scale != glm::vec2(1.0f, 1.0f))
        };
    }
};

using Material = BasicMaterial<std::shared_ptr<Texture>>;
