#pragma once

#include "FPSMeter.hpp"
#include "nodes/gui/TextNode.hpp"

class FPSTextNode : public llengine::TextNode {
public:
    FPSTextNode();
    void update() override final;

private:
    llengine::FPSMeter fps_meter;
};