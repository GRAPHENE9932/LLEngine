#include "FadingPointLightNode.hpp"
#include "LLEngine/rendering/RenderingServer.hpp"

void FadingPointLightNode::update() {
    if (time_left <= 0.0f) {
        get_parent()->queue_remove_child(this);
        return;
    }

    this->color = get_original_color() * (time_left / duration);
    time_left -= llengine::rs().get_delta_time();
}

void FadingPointLightNode::set_duration(float new_duration) {
    this->duration = new_duration;
    this->time_left = new_duration;
}

[[nodiscard]] float FadingPointLightNode::get_duration() const {
    return duration;
}

[[nodiscard]] float FadingPointLightNode::get_time_left() const {
    return time_left;
}

void FadingPointLightNode::set_original_color(const glm::vec3 new_original_color) {
    this->original_color = new_original_color;
}

[[nodiscard]] glm::vec3 FadingPointLightNode::get_original_color() const {
    return original_color;
}

void FadingPointLightNode::copy_to(llengine::Node& node) const {
    llengine::PointLightNode::copy_to(node);

    FadingPointLightNode& fpl_node = dynamic_cast<FadingPointLightNode&>(node);
    fpl_node.set_duration(get_duration());
    fpl_node.set_original_color(get_original_color());
    fpl_node.time_left = time_left;
}

[[nodiscard]] std::unique_ptr<llengine::Node> FadingPointLightNode::copy() const {
    std::unique_ptr<FadingPointLightNode> result {std::make_unique<FadingPointLightNode>()};
    copy_to(*result);
    return result;
}
