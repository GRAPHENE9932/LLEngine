#pragma once

#include <array> // std::array
#include <memory> // std::shared_ptr
#include <optional> // std::optional
#include <algorithm>

#include <glm/vec2.hpp> // glm::vec2
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <utility>

#include "Texture.hpp" // Texture
#include "utils/Channel.hpp"

namespace llengine {
template<typename TEX_TYPE>
struct BasicMaterial {
    struct TextureInfo {
        TEX_TYPE texture;
        glm::vec2 uv_offset = {0.0f, 0.0f};
        glm::vec2 uv_scale = {1.0f, 1.0f};

        [[nodiscard]] bool has_default_offset_and_scale() const noexcept {
            return uv_offset == glm::vec2(0.0f, 0.0f) && uv_scale == glm::vec2(1.0f, 1.0f);
        }
    };

    struct NormalMap {
        TextureInfo texture;
        float scale = 1.0f;
    };

    struct SingleChannelTextureInfo : public TextureInfo {
        Channel channel;
    };

    std::optional<TextureInfo> base_color_texture = std::nullopt;
    glm::vec4 base_color_factor = {1.0f, 1.0f, 1.0f, 1.0f};

    std::optional<TextureInfo> emissive_texture = std::nullopt;
    glm::vec3 emissive_factor = {0.0f, 0.0f, 0.0f};

    std::optional<SingleChannelTextureInfo> ambient_occlusion_texture = std::nullopt;
    float ambient_occlusion_factor = 1.0f;

    std::optional<SingleChannelTextureInfo> metallic_texture = std::nullopt;
    float metallic_factor = 1.0f;

    std::optional<SingleChannelTextureInfo> roughness_texture = std::nullopt;
    float roughness_factor = 1.0f;
    
    std::optional<NormalMap> normal_map = std::nullopt;

    [[nodiscard]] bool has_offsets_and_scales() const noexcept {
        const auto textures = get_array_of_textures();
        return std::any_of(
            textures.begin(), textures.end(),
            [] (const TextureInfo* tex_info) -> bool {
                return tex_info && !tex_info->has_default_offset_and_scale();
            }
        );
    }

    [[nodiscard]] bool has_identical_offsets_and_scales() const noexcept {
        const auto textures_array = get_array_of_textures();

        // Check if all offsets and scales are equal (don't check nullopt objects).
        const TextureInfo* previous = nullptr;
        for (size_t i = 0; i < textures_array.size(); i++) {
            const TextureInfo* current = textures_array[i];

            if (current) {
                if (previous &&
                    (previous->uv_offset != current->uv_offset ||
                    previous->uv_scale != current->uv_scale)) {
                    return false;
                }

                previous = current;
            }
        }

        return true;
    }

    /// The material must have identical UV offsets and scales, otherwise
    /// result is undefined.
    /// @returns Pair of two vectors: offset and scale.
    [[nodiscard]] std::pair<glm::vec2, glm::vec2> get_general_uv_offset_and_scale() const noexcept {
        assert(has_identical_offsets_and_scales());

        // In this function we assume that all uv_offsets and uv_scales are identical
        // between base color texture, metallic-roughness texture, normal map, etc.
        // So, just return any of them.
        const auto textures_array = get_array_of_textures();
        const auto iter = std::find_if(
            textures_array.begin(), textures_array.end(),
            [] (const TextureInfo* tex_info) -> bool {
                return static_cast<bool>(tex_info);
            }
        );
        if (iter == textures_array.end()) {
            return {{0.0f, 0.0f}, {1.0f, 1.0f}};
        }
        return std::make_pair(
            (*iter)->uv_offset, (*iter)->uv_scale
        );
    }

private:
    inline std::array<const TextureInfo*, 6> get_array_of_textures() const noexcept {
        return {
            static_cast<const TextureInfo* const>(base_color_texture.has_value() ? &(*base_color_texture) : nullptr),
            static_cast<const TextureInfo* const>(emissive_texture.has_value() ? &(*emissive_texture) : nullptr),
            static_cast<const TextureInfo* const>(ambient_occlusion_texture.has_value() ? &(*ambient_occlusion_texture) : nullptr),
            static_cast<const TextureInfo* const>(metallic_texture.has_value() ? &(*metallic_texture) : nullptr),
            static_cast<const TextureInfo* const>(roughness_texture.has_value() ? &(*roughness_texture) : nullptr),
            static_cast<const TextureInfo* const>(normal_map.has_value() ? &(normal_map->texture) : nullptr)
        };
    }
};

using Material = BasicMaterial<std::shared_ptr<Texture>>;
}