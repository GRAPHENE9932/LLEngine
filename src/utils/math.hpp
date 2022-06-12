#pragma once

#include "../consts_and_enums.hpp"
#include "../structs/Rect.hpp"
#include "../structs/VertLS.hpp"
#include "../structs/HorLS.hpp"

namespace utils {
    Rect window_space_to_opengl_space(Rect rect, glm::vec2 window_size);
}
