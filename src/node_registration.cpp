#include "node_registration.hpp"

#include <map>

using namespace llengine;

std::map<std::string, CustomNodeType> custom_nodes_map;

void add_node_factory(const std::string& node_type_name, CustomNodeFactory&& factory) {
    custom_nodes_map[std::string(node_type_name)].factory = std::move(factory);
}

void add_custom_property(
    const std::string& node_type_name, CustomPropertyType type,
    const std::string& property_name, void(*abstract_setter_ptr)(SpatialNode*)
) {
    custom_nodes_map.find(node_type_name)->second.properties.push_back({
        type, property_name, abstract_setter_ptr
    });
}

template<typename T>
void register_custom_property(
    const std::string& node_type_name,
    const std::string& property_name,
    void(*setter_ptr)(SpatialNode*, T)
) {
    if (!custom_nodes_map.contains(node_type_name)) {
        throw std::runtime_error(
            "Failed to register custom property because the custom node type is not registered yet."
        );
    }

    custom_nodes_map[node_type_name].properties.push_back({get_custom_property_type<T>(), property_name});
}