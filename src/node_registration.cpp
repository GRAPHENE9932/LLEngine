#include "node_registration.hpp"

#include <fmt/format.h>
#include <logger.hpp>

#include <map>

using namespace llengine;
using namespace llengine::internal;

enum NodeRegistrationState : std::uint8_t {
    REGISTRATION_NOT_BEGUN,
    REGISTRATION_BEGUN,
    REGISTRATION_ENDED
};

std::map<std::string, CustomNodeType> custom_nodes_map;
NodeRegistrationState node_registration_state = REGISTRATION_NOT_BEGUN;

[[nodiscard]] std::unique_ptr<Node> CustomNodeType::construct_node() const {
    return holder->construct();
}

void CustomNodeType::add_setter(std::string_view name, PropertySetter&& setter) {
    property_setters.insert(std::make_pair(name, std::move(setter)));
}

bool CustomNodeType::call_setter(Node& node, const NodeProperty& property) {
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

void llengine::begin_nodes_registration() {
    if (node_registration_state == REGISTRATION_BEGUN) {
        throw std::runtime_error("Can not begin nodes registration because it was already begun.");
    }
    else if  (node_registration_state == REGISTRATION_ENDED) {
        throw std::runtime_error("Registration can be started only once.");
    }

    node_registration_state = REGISTRATION_BEGUN;
    register_builtin_nodes();
}

void find_parents_for_every_type() {
    for (auto& current : custom_nodes_map) {
        auto iter_to_parent = std::find_if(
            custom_nodes_map.begin(), custom_nodes_map.end(),
            [&current] (const auto& parent) {
                return parent.second.is_parent_of(current.second);
            }
        );

        if (iter_to_parent != custom_nodes_map.end()) {
            current.second.assign_parent(&iter_to_parent->second);
        }
        else {
            current.second.assign_parent(nullptr);
        }
    }
}

void llengine::end_nodes_registration() {
    if (node_registration_state == REGISTRATION_NOT_BEGUN) {
        throw std::runtime_error("Can not end nodes registration because it hasn't begun.");
    }
    else if (node_registration_state == REGISTRATION_ENDED) {
        throw std::runtime_error("Can not end nodes registration because it was already over.");
    }

    find_parents_for_every_type();

    node_registration_state = REGISTRATION_ENDED;
}

void internal::add_node_type(std::string_view node_type_name, CustomNodeType&& node_type) {
    custom_nodes_map.insert(std::make_pair(node_type_name, std::move(node_type)));
}

void internal::add_node_setter(std::string_view node_type_name, std::string_view property_name, PropertySetter&& setter) {
    auto iter = custom_nodes_map.find(std::string(node_type_name));
    if (iter == custom_nodes_map.end()) {
        throw std::runtime_error(fmt::format(
            "Can't find a node with name \"{}\"", node_type_name
        ));
    }

    iter->second.add_setter(property_name, std::move(setter));
}

[[nodiscard]] std::unique_ptr<Node> llengine::construct_node(std::string_view node_type_name) {
    return custom_nodes_map.at(std::string(node_type_name)).construct_node();
}

[[nodiscard]] std::unique_ptr<Node> llengine::construct_node(std::string_view node_type_name, const std::vector<NodeProperty>& properties) {
    CustomNodeType& type = custom_nodes_map.at(std::string(node_type_name));
    std::unique_ptr<Node> result = type.construct_node();
    for (const auto& property : properties) {
        if (!type.call_setter(*result, property)) {
            logger::warning(fmt::format(
                "Tried to set inexistent/unregistered property \"{}\" to a node of type \"{}\"",
                property.get_name(), node_type_name
            ));
        }
    }
    return result;
}

void llengine::call_setter(std::string_view node_type_name, const NodeProperty& property, Node& node) {
    custom_nodes_map.at(std::string(node_type_name)).call_setter(node, property);
}

void llengine::set_properties_to_node(Node& node, const std::vector<NodeProperty>& properties) {
    std::optional<std::string_view> node_name = find_node_type_name(node);
    if (!node_name.has_value()) {
        logger::warning("Tried to set properties to a node of unregistered type.");
        return;
    }

    for (const NodeProperty& property : properties) {
        call_setter(*node_name, property, node);
    }
}

[[nodiscard]] std::optional<std::string_view> llengine::find_node_type_name(const Node& node) {
    const auto iter = std::find_if(
        custom_nodes_map.begin(), custom_nodes_map.end(),
        [&node] (const auto& pair) {
            return pair.second.get_type_index() == typeid(node);
        }
    );

    if (iter == custom_nodes_map.end()) {
        return std::nullopt;
    }

    return iter->first;
}