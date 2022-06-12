#pragma once

#include "../structs/Rect.hpp"

namespace utils {
    Rect window_space_to_opengl_space(Rect rect, glm::vec2 window_size);
}
