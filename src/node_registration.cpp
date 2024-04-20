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

static void find_parents_for_every_type() {
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

const CustomNodeType& internal::find_custom_node_type_by_type_index(std::type_index type_index) {
    const auto iter = std::find_if(
        custom_nodes_map.begin(), custom_nodes_map.end(),
        [&type_index] (const auto& pair) {
            return pair.second.get_type_index() == type_index;
        }
    );

    if (iter == custom_nodes_map.end()) {
        throw std::runtime_error("Tried to find a non-existent custom node type.");
    }

    return iter->second;
}

[[nodiscard]] std::unique_ptr<Node> llengine::construct_node(std::string_view node_type_name, const std::vector<NodeProperty>& properties) {
    CustomNodeType& type = custom_nodes_map.at(std::string(node_type_name));
    return construct_node(type, properties);
}

[[nodiscard]] std::unique_ptr<Node> llengine::construct_node(const CustomNodeType& node_type, const std::vector<NodeProperty>& properties) {
    std::unique_ptr<Node> result = node_type.construct_node();
    for (const auto& property : properties) {
        if (!node_type.call_setter(*result, property)) {
            logger::warning(fmt::format(
                "Tried to set inexistent/unregistered property \"{}\" to a node.",
                property.get_name()
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