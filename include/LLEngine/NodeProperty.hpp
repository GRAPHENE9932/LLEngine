#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <vector>
#include <string>
#include <memory>
#include <cstdint>
#include <variant>
#include <optional>
#include <string_view>

namespace llengine {
class NodeProperty {
public:
    template<typename T>
    NodeProperty(std::string_view name, T&& value) : name(name), property(std::forward<T>(value)) {}

    NodeProperty(const NodeProperty& other) = default;
    NodeProperty(NodeProperty&& other) = default;

    template<typename T>
    [[nodiscard]] T get() const;

    [[nodiscard]] inline const std::vector<NodeProperty>& get_subproperties() const {
        return std::get<std::vector<NodeProperty>>(property);
    }

    [[nodiscard]] const NodeProperty& get_subproperty(std::string_view subproperty_name) const;
    [[nodiscard]] const NodeProperty* get_optional_subproperty(std::string_view subproperty_name) const;

    template<typename T>
    [[nodiscard]] T get(std::string_view subproperty_name) const {
        return get_subproperty(subproperty_name).get<T>();
    }

    template<typename T>
    [[nodiscard]] std::optional<T> get_optional(std::string_view subproperty_name) const {
        const NodeProperty* ptr = get_optional_subproperty(subproperty_name);
        if (ptr == nullptr) {
            return std::nullopt;
        }
        else {
            return ptr->get<T>();
        }
    }

    template<typename T>
    [[nodiscard]] bool holds() const {
        return std::holds_alternative<T>(property);
    }

    template<typename T>
    void set(T&& value) {
        property = std::forward<T>(value);
    }

    [[nodiscard]] const std::string& get_name() const {
        return name;
    }

    void set_name(std::string_view new_name) {
        name = new_name;
    }

private:
    std::string name;
    std::variant<
        std::int64_t, float, bool, std::string,
        std::vector<std::int64_t>, std::vector<float>, std::vector<glm::vec2>,
        std::vector<glm::vec3>, std::vector<glm::vec4>, std::vector<glm::i32vec2>,
        std::vector<std::string>, std::vector<NodeProperty>
    > property;
};
}