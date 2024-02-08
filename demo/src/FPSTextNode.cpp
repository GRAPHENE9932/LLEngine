#include "FPSTextNode.hpp"
#include "rendering/RenderingServer.hpp"

#include <fmt/format.h>

constexpr float INTERVAL = 0.5f;

void FPSTextNode::update() {
    accumulated_time += get_rendering_server().get_delta_time();
    counted_frames++;

    set_text(fmt::format(
        "{:0.1f} FPS",
        latest_value
    ));

    if (accumulated_time >= INTERVAL) {
        latest_value = counted_frames / accumulated_time;
        accumulated_time = 0.0f;
        counted_frames = 0;
    }
}