#pragma once

#include "nodes/Node.hpp"
#include "CustomNodeType.hpp"

#include <functional>
#include <typeindex>
#include <memory>
#include <vector>

namespace llengine {
namespace internal {
void add_node_type(std::string_view node_type_name, CustomNodeType&& node_type);
void add_node_setter(std::string_view node_type_name, std::string_view property_name, PropertySetter&& setter);
void register_builtin_nodes();
const CustomNodeType& find_custom_node_type_by_type_index(std::type_index type_index);

template<typename T>
concept has_register_properties = std::is_same_v<decltype(T::register_properties()), void>;

template<typename T>
void register_baseless_node_type(std::string_view node_type_name) {
    internal::add_node_type(node_type_name, CustomNodeType::create_from_type<T>());
    if constexpr (internal::has_register_properties<T>) {
        T::register_properties();
    }
}
}

void begin_nodes_registration();
void end_nodes_registration();

template<typename T, typename Base>
void register_node_type(std::string_view node_type_name) {
    internal::add_node_type(node_type_name, CustomNodeType::create_from_type<T, Base>());
    if constexpr (internal::has_register_properties<T>) {
        T::register_properties();
    }
}

template<typename T>
void register_custom_property(
    std::string_view node_type_name,
    std::string_view property_name,
    std::function<void(T&, const NodeProperty&)> setter
) {
    internal::add_node_setter(node_type_name, property_name, internal::PropertySetter::create<T>(setter));
}

[[nodiscard]] std::unique_ptr<Node> construct_node(std::string_view node_type_name, const std::vector<NodeProperty>& properties = {});
[[nodiscard]] std::unique_ptr<Node> construct_node(const CustomNodeType& node_type, const std::vector<NodeProperty>& properties = {});
void call_setter(std::string_view node_type_name, const NodeProperty& property, Node& node);
void set_properties_to_node(Node& node, const std::vector<NodeProperty>& properties);
[[nodiscard]] std::optional<std::string_view> find_node_type_name(const Node& node);
template<typename T>
const CustomNodeType& find_custom_node_type() {
    return internal::find_custom_node_type_by_type_index(typeid(T));
}
}