#pragma once

#include "rendering/Texture.hpp"

namespace llengine::tex_utils {
[[nodiscard]] Texture panorama_to_cubemap(const Texture& panorama);
[[nodiscard]] Texture compute_irradiance_map(const Texture& cubemap);
[[nodiscard]] Texture compute_prefiltered_specular_map(const Texture& cubemap);
[[nodiscard]] Texture compute_brdf_integration_map();
}