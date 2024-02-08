#include "ResolutionTextNode.hpp"

#include <LLEngine/rendering/RenderingServer.hpp>

#include <fmt/format.h>

void ResolutionTextNode::update() {
    const auto size = get_rendering_server().get_window().get_window_size();

    set_text(fmt::format(
        "Resolution: {}x{}",
        size.x,
        size.y
    ));
}