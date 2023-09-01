#include "CustomNodeType.hpp"

namespace llengine {
[[nodiscard]] std::unique_ptr<Node> CustomNodeType::construct_node() const {
    return holder->construct();
}

void CustomNodeType::add_setter(std::string_view name, internal::PropertySetter&& setter) {
    property_setters.insert(std::make_pair(name, std::move(setter)));
}

bool CustomNodeType::call_setter(Node& node, const NodeProperty& property) const {
    auto iter = property_setters.find(std::string(property.get_name()));
    if (iter != property_setters.end()) {
        iter->second.call(node, property);
        return true;
    }

    if (auto parent = get_parent()) {
        return parent->call_setter(node, property);
    }

    return false;
}

[[nodiscard]] bool CustomNodeType::has_setter_for(std::string_view name) const {
    if (property_setters.contains(std::string(name))) {
        return true;
    }

    if (auto parent = get_parent()) {
        return parent->has_setter_for(name);
    }

    return false;
}

[[nodiscard]] bool CustomNodeType::has_own_setter_for(std::string_view name) const {
    return property_setters.contains(std::string(name));
}

[[nodiscard]] bool CustomNodeType::is_parent_of(const CustomNodeType& other) const {
    if (std::holds_alternative<CustomNodeType*>(other.parent)) {
        if (std::get<CustomNodeType*>(other.parent) == nullptr) {
            return false;
        }

        return *this == *std::get<CustomNodeType*>(other.parent);
    }
    else {
        return this->get_type_index() == std::get<std::type_index>(other.parent);
    }
}

[[nodiscard]] std::type_index CustomNodeType::get_type_index() const {
    return holder->get_type_info();
}

void CustomNodeType::assign_parent(CustomNodeType* parent) {
    this->parent = parent;
}

[[nodiscard]] CustomNodeType* CustomNodeType::get_parent() const {
    return std::get<CustomNodeType*>(parent);
}

[[nodiscard]] bool CustomNodeType::operator==(const CustomNodeType& other) const {
    return holder->get_type_info() == other.holder->get_type_info();
}

[[nodiscard]] bool CustomNodeType::operator!=(const CustomNodeType &other) const {
    return !(*this == other);
}
}