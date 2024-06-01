#include "rendering/LightingEnvironment.hpp"
#include "utils/texture_utils.hpp"

namespace llengine {
void LightingEnvironment::set_base_cubemap(const std::shared_ptr<Texture>& new_base_map) {
    if (new_base_map == nullptr) {
        return;
    }

    if (base_map != nullptr && *base_map == *new_base_map) {
        return;
    }

    base_map = new_base_map;
    irradiance_map = std::nullopt;
    prefiltered_specular_map = std::nullopt;
}

[[nodiscard]] std::shared_ptr<const Texture> LightingEnvironment::get_base_cubemap() const {
    return base_map;
}

[[nodiscard]] const std::optional<Texture>& LightingEnvironment::get_irradiance_map() const {
    if (base_map != nullptr && !irradiance_map.has_value()) {
        irradiance_map = tex_utils::compute_irradiance_map(*base_map);
    }

    return irradiance_map;
}

[[nodiscard]] const std::optional<Texture>& LightingEnvironment::get_prefiltered_specular_map() const {
    if (base_map != nullptr && !prefiltered_specular_map.has_value()) {
        prefiltered_specular_map = tex_utils::compute_prefiltered_specular_map(*base_map);
    }

    return prefiltered_specular_map;
}
}
