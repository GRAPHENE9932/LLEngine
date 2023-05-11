#pragma once

#include "GUINode.hpp"

class CompleteGUINode : public GUINode {
public:
    explicit CompleteGUINode(RenderingServer& rs) : GUINode(rs) {}

    [[nodiscard]] virtual GUITransform get_transform() const override;
    [[nodiscard]] virtual glm::vec2 get_absolute_size() const override;
    virtual void set_transform(const GUITransform& transform) override;

private:
    GUITransform transform;
};