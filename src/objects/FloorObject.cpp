#include "FloorObject.hpp"

FloorObject::FloorObject(Rect rect, float height) : height(height), rect(rect) {

}

bool FloorObject::circle_over_floor(glm::vec2 position, float radius) const {
    float distance = rect.distance_from(position);
    return distance <= radius;
}
