#pragma once

#include <LLEngine/nodes/gui/TextNode.hpp>

class ResolutionTextNode : public llengine::TextNode {
public:
    void update() final override;
};