#include "ExposureTextNode.hpp"
#include "rendering/RenderingServer.hpp"

#include <fmt/format.h>

void ExposureTextNode::update() {
    set_text(fmt::format(
        "Exposure: {:0.3f}", llengine::rs().get_exposure()
    ));
}