#include <glm/gtx/compatibility.hpp>

#include "MovingLightBulb.hpp"

MovingLightBulb::MovingLightBulb(const std::vector<glm::vec3>& path, const float speed) :
        path(path), speed(speed) {
    assert(path.size() >= 3);

    full_length = 0.0f;
    for (std::size_t i = 1; i < path.size(); i++)
        full_length += glm::distance(path[i], path[i - 1]);

    cur_segment_len = glm::distance(path[cur_segment], path[cur_segment + 1]);
}

void MovingLightBulb::update(const float delta) {
    // Add some length.
    passed_len_in_segment += delta * speed;

    // Increment cur_segment if we should go to the next segment.
    if (passed_len_in_segment > cur_segment_len)
        if (++cur_segment > path.size() - 1)
            cur_segment = 0;

    std::size_t next_segment {cur_segment == path.size() - 1 ? 0 : cur_segment + 1};

    // Update info for next line segment.
    if (passed_len_in_segment > cur_segment_len) {
        passed_len_in_segment -= cur_segment_len;
        cur_segment_len = glm::distance(path[cur_segment], path[next_segment]);
        passed_len_in_segment = std::fmod(passed_len_in_segment, cur_segment_len);
    }

    // Compute position.
    const float phase {passed_len_in_segment / cur_segment_len};
    const glm::vec3 new_pos {glm::lerp(path[cur_segment], path[next_segment], phase)};

    // Set position.
    if (std::shared_ptr<PointLight> ptr = point_light.lock())
        ptr->position = new_pos;
    if (std::shared_ptr<UnshadedDrawableObject> ptr = drawable_obj.lock())
        ptr->translation = new_pos;
}
