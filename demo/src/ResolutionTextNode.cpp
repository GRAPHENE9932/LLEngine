#include "ResolutionTextNode.hpp"

#include <LLEngine/rendering/RenderingServer.hpp>

#include <fmt/format.h>

void ResolutionTextNode::update() {
    const auto size = llengine::rs().get_window().get_framebuffer_size();

    set_text(fmt::format(
        "Resolution: {}x{}",
        size.x,
        size.y
    ));
}