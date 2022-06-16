#include <array>
#include <algorithm>

#include <glm/trigonometric.hpp>
#include <glm/geometric.hpp>

#include "../structs/HorLS.hpp"
#include "../structs/VertLS.hpp"
#include "../structs/QuadrantArc.hpp"
#include "math.hpp"

Rect utils::window_space_to_opengl_space(Rect rect, glm::vec2 window_size) {
    rect.position = rect.position / window_size * 2.0f;
    rect.position -= glm::vec2(1.0f, 1.0f);

    rect.size /= window_size;
    rect.size *= 2.0f;

    return rect;
}
