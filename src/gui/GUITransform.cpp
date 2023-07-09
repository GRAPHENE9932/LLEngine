#include "gui/GUITransform.hpp"
#include "NodeProperty.hpp"

#include <fmt/format.h>

using namespace llengine;

[[nodiscard]] glm::vec3 GUITransform::get_screen_space_offset(glm::vec2 parent_size) const noexcept {
    glm::vec2 result_vec2 {position_offset};
    result_vec2 += position_anchor * static_cast<glm::vec2>(parent_size);

    if (origin_x == OriginX::CENTER) {
        result_vec2.x -= size.x / 2.0f;
    }
    else if (origin_x == OriginX::RIGHT) {
        result_vec2.x -= size.x;
    }

    if (origin_y == OriginY::CENTER) {
        result_vec2.y -= size.y / 2.0f;
    }
    else if (origin_y == OriginY::BOTTOM) {
        result_vec2.y -= size.y;
    }

    return {result_vec2, z_coordinate};
}

[[nodiscard]] GUITransform::OriginX origin_x_from_string(std::string_view string) {
    if (string == "left") {
        return GUITransform::OriginX::LEFT;
    }
    else if (string == "center") {
        return GUITransform::OriginX::CENTER;
    }
    else if (string == "right") {
        return GUITransform::OriginX::RIGHT;
    }
    else {
        throw std::runtime_error(fmt::format(
            "Can't convert string \"{}\" to origin x. Only left, center and right are valid strings.",
            string
        ));
    }
}

[[nodiscard]] GUITransform::OriginY origin_y_from_string(std::string_view string) {
    if (string == "bottom") {
        return GUITransform::OriginY::BOTTOM;
    }
    else if (string == "center") {
        return GUITransform::OriginY::CENTER;
    }
    else if (string == "top") {
        return GUITransform::OriginY::TOP;
    }
    else {
        throw std::runtime_error(fmt::format(
            "Can't convert string \"{}\" to origin y. Only bottom, center and top are valid strings.",
            string
        ));
    }
}

[[nodiscard]] GUITransform::SizeMode size_mode_from_string(std::string_view string) {
    if (string == "absolute") {
        return GUITransform::SizeMode::ABSOLUTE;
    }
    else if (string == "relative") {
        return GUITransform::SizeMode::RELATIVE;
    }
    else {
        throw std::runtime_error(fmt::format(
            "Can't convert string \"{}\" to size mode. Only absolute and relative are valid strings.",
            string
        ));
    }
}

[[nodiscard]] GUITransform GUITransform::from_property(const NodeProperty& property) {
    std::string type = property.get_subproperty("type").get<std::string>();
    if (type != "gui_transform") {
        throw std::runtime_error(fmt::format(
            "Failed to convert node property into GUI transform: invalid type: \"{}\"",
            type
        ));
    }

    GUITransform transform;

    transform.position_anchor = property.get_optional<glm::vec2>("position_anchor").value_or(glm::vec2(0.0f, 0.0f));
    transform.position_offset = property.get_optional<glm::vec2>("position_offset").value_or(glm::vec2(0.0f, 0.0f));
    transform.z_coordinate = property.get_optional<float>("z_coordinate").value_or(0.0f);
    transform.origin_x = origin_x_from_string(property.get_optional<std::string>("origin_x").value_or("left"));
    transform.origin_y = origin_y_from_string(property.get_optional<std::string>("origin_y").value_or("bottom"));
    transform.size_mode = size_mode_from_string(property.get_optional<std::string>("size_mode").value_or("absolute"));
    transform.size = property.get_optional<glm::vec2>("size").value_or(glm::vec2(0.0f, 0.0f));

    return transform;
}