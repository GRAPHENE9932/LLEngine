#pragma once

#include "QualitySettings.hpp"

#include <glm/vec2.hpp>

#include <string>

namespace llengine {
struct GameSettings {
    std::string window_title;
    std::string json_scene_path;
    std::string skybox_path;
    glm::ivec2 window_resolution;
    QualitySettings quality_settings;
};
}