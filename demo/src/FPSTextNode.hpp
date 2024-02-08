#pragma once

#include "nodes/gui/TextNode.hpp"

class FPSTextNode : public llengine::TextNode {
public:
    FPSTextNode() = default;
    void update() override final;

private:
    float accumulated_time = 0.0f;
    std::size_t counted_frames = 0;
    float latest_value = 0.0f;
};