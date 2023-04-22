#include "RectangularGUINode.hpp"

[[nodiscard]] glm::vec2 RectangularGUINode::get_origin() const {
    const glm::vec2 size = get_size();
    glm::vec2 result;

    switch (origin_x) {
    case OriginX::LEFT:
        result.x = 0.0f;
        break;
    case OriginX::CENTER:
        result.x = size.x / 2.0f;
        break;
    case OriginX::RIGHT:
        result.x = size.x;
        break;
    }

    switch (origin_y) {
    case OriginY::BOTTOM:
        result.y = 0.0f;
        break;
    case OriginY::CENTER:
        result.y = size.y / 2.0f;
        break;
    case OriginY::TOP:
        result.y = size.y;
        break;
    }

    return result;
}

[[nodiscard]] bool RectangularGUINode::contains_point(glm::vec2 point) const {
    glm::vec2 top_left = static_cast<glm::vec2>(get_absolute_transform().to_screen_coordinates(
        rs.get_window().get_window_size()
    )) + get_origin();

    glm::vec2 bottom_right = top_left + get_size();

    return point.x >= top_left.x && point.x <= bottom_right.x &&
        point.y >= top_left.y && point.y <= bottom_right.y;
}
