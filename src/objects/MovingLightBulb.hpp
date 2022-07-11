#pragma once

#include <vector>
#include <memory>

#include <glm/vec3.hpp>

#include "PointLight.hpp"
#include "UnshadedDrawableObject.hpp"

class MovingLightBulb {
public:
    std::weak_ptr<PointLight> point_light;
    std::weak_ptr<UnshadedDrawableObject> drawable_obj;

    MovingLightBulb(const std::vector<glm::vec3>& path, const float speed);

    void update(const float delta);

private:
    std::vector<glm::vec3> path;
    float full_length;
    float speed;

    float passed_len_in_segment = 0.0f;
    float cur_segment_len;
    std::size_t cur_segment = 0;
};
