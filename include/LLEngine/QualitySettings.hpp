#pragma once

#include <glm/vec2.hpp>

namespace llengine {
struct QualitySettings {
    bool shadow_mapping_enabled = true;
    glm::u32vec2 shadow_map_size = {1024, 1024};
    float shadow_map_drawing_distance = 20.0f;

    bool enable_bloom = true;
};
}