#pragma once

#include "Texture.hpp"

#include <memory>
#include <optional>

namespace llengine {
class LightingEnvironment {
public:
    void set_base_cubemap(const std::shared_ptr<Texture>& new_base_map);
    [[nodiscard]] std::shared_ptr<const Texture> get_base_cubemap() const;
    [[nodiscard]] const std::optional<Texture>& get_irradiance_map() const;
    [[nodiscard]] const std::optional<Texture>& get_prefiltered_specular_map() const;

private:
    std::shared_ptr<Texture> base_map = nullptr;
    mutable std::optional<Texture> irradiance_map = std::nullopt;
    mutable std::optional<Texture> prefiltered_specular_map = std::nullopt;
};
}
