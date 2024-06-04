#pragma once

#include "LLEngine/nodes/rendering/PointLightNode.hpp"

class FadingPointLightNode : public llengine::PointLightNode {
public:
    void update() final override;

    void set_duration(float new_duration);
    [[nodiscard]] float get_duration() const;
    [[nodiscard]] float get_time_left() const;

    void set_original_color(const glm::vec3 new_original_color);
    [[nodiscard]] glm::vec3 get_original_color() const;

    void copy_to(llengine::Node& node) const override;
    [[nodiscard]] std::unique_ptr<llengine::Node> copy() const override;
private:
    float time_left = 0.0f;
    float duration = 0.0f;
    glm::vec3 original_color = {0.0f, 0.0f, 0.0f};
};
