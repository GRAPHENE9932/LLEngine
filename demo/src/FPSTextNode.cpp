#include "FPSTextNode.hpp"

#include <fmt/format.h>

FPSTextNode::FPSTextNode() : fps_meter(0.5f) {
    
}

void FPSTextNode::update() {
    fps_meter.frame();
    set_text(fmt::format(
        "{:0.1f} FPS",
        fps_meter.get_fps()
    ));
}