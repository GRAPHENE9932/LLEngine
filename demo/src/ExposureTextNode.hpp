#pragma once

#include "nodes/gui/TextNode.hpp"

class ExposureTextNode : public llengine::TextNode {
public:
    void update() final override;
};