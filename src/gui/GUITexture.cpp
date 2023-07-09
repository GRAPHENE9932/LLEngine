#include "gui/GUITexture.hpp"
#include "NodeProperty.hpp"

using namespace llengine;

[[nodiscard]] GUITexture GUITexture::from_property(const NodeProperty& property) {
    glm::vec4 borders { property.get<glm::vec4>("borders") };
    return {
        std::make_shared<Texture>(std::move(Texture::from_property(property.get_subproperty("texture")))),
        borders.x, borders.y, borders.z, borders.w,
        property.get<glm::vec4>("color_factor")
    };
}