#include "NodeProperty.hpp"

#include <fmt/format.h>
#include <glm/gtx/quaternion.hpp>

#include <algorithm>

using namespace llengine;

template<typename T>
[[nodiscard]] T NodeProperty::get() const {
    return std::get<T>(property);
}
template std::string NodeProperty::get<std::string>() const;
template std::vector<std::int64_t> NodeProperty::get<std::vector<std::int64_t>>() const;
template std::vector<float> NodeProperty::get<std::vector<float>>() const;
template std::vector<glm::vec3> NodeProperty::get<std::vector<glm::vec3>>() const;
template std::vector<glm::vec4> NodeProperty::get<std::vector<glm::vec4>>() const;
template std::vector<glm::i32vec2> NodeProperty::get<std::vector<glm::i32vec2>>() const;
template std::vector<std::string> NodeProperty::get<std::vector<std::string>>() const;
template std::vector<NodeProperty> NodeProperty::get<std::vector<NodeProperty>>() const;

template<>
[[nodiscard]] std::int64_t NodeProperty::get() const {
    if (std::holds_alternative<bool>(property)) {
        return static_cast<bool>(std::get<bool>(property));
    }
    else {
        return std::get<std::int64_t>(property);
    }
}

template<>
[[nodiscard]] std::int8_t NodeProperty::get() const {
    return static_cast<std::int8_t>(get<std::int64_t>());
}
template<>
[[nodiscard]] std::uint8_t NodeProperty::get() const {
    return static_cast<std::uint8_t>(get<std::int64_t>());
}
template<>
[[nodiscard]] std::int16_t NodeProperty::get() const {
    return static_cast<std::int8_t>(get<std::int64_t>());
}
template<>
[[nodiscard]] std::uint16_t NodeProperty::get() const {
    return static_cast<std::uint8_t>(get<std::int64_t>());
}
template<>
[[nodiscard]] std::int32_t NodeProperty::get() const {
    return static_cast<std::int8_t>(get<std::int64_t>());
}
template<>
[[nodiscard]] std::uint32_t NodeProperty::get() const {
    return static_cast<std::uint8_t>(get<std::int64_t>());
}
template<>
[[nodiscard]] std::uint64_t NodeProperty::get() const {
    return static_cast<std::int8_t>(get<std::int64_t>());
}

template<>
[[nodiscard]] float NodeProperty::get() const {
    if (std::holds_alternative<std::int64_t>(property)) {
        return static_cast<float>(std::get<std::int64_t>(property));
    }
    else {
        return std::get<float>(property);
    }
}

template<>
[[nodiscard]] bool NodeProperty::get() const {
    if (std::holds_alternative<std::int64_t>(property)) {
        return static_cast<bool>(std::get<std::int64_t>(property));
    }
    else {
        return std::get<bool>(property);
    }
}

template<glm::length_t L>
[[nodiscard]] glm::vec<L, float> get_vec_from_variant(
    const std::variant<
        std::int64_t, float, bool, std::string,
        std::vector<std::int64_t>, std::vector<float>, std::vector<glm::vec2>,
        std::vector<glm::vec3>, std::vector<glm::vec4>, std::vector<glm::i32vec2>,
        std::vector<std::string>, std::vector<NodeProperty>
    >& variant
) {
    if (std::holds_alternative<std::vector<std::int64_t>>(variant)) {
        const auto& vector = std::get<std::vector<std::int64_t>>(variant);

        glm::vec<L, float> result;
        for (glm::length_t i = 0; i < L; i++) {
            result[i] = static_cast<float>(vector.at(i));
        }
 
        return result;
    }
    else {
        const auto& vector = std::get<std::vector<float>>(variant);

        glm::vec<L, float> result;
        for (glm::length_t i = 0; i < L; i++) {
            result[i] = vector.at(i);
        }

        return result;
    }
}

template<>
[[nodiscard]] glm::vec2 NodeProperty::get() const {
    return get_vec_from_variant<2>(property);
}

template<>
[[nodiscard]] glm::vec3 NodeProperty::get() const {
    return get_vec_from_variant<3>(property);
}

template<>
[[nodiscard]] glm::vec4 NodeProperty::get() const {
    return get_vec_from_variant<4>(property);
}

template<>
[[nodiscard]] glm::i32vec2 NodeProperty::get() const {
    const auto& vector = std::get<std::vector<std::int64_t>>(property);
    return {
        static_cast<std::int32_t>(vector[0]),
        static_cast<std::int32_t>(vector[1])
    };
}

template<>
[[nodiscard]] glm::quat NodeProperty::get() const {
    glm::vec4 vec4 = get<glm::vec4>();
    return glm::quat(vec4.w, vec4.x, vec4.y, vec4.z);
}

template<>
[[nodiscard]] std::vector<glm::vec2> NodeProperty::get() const {
    if (std::holds_alternative<std::vector<glm::i32vec2>>(property)) {
        const std::vector<glm::i32vec2> ints = std::get<std::vector<glm::i32vec2>>(property);
        std::vector<glm::vec2> floats(ints.size());
        std::transform(
            ints.begin(), ints.end(), floats.begin(),
            [] (glm::i32vec2 input) -> glm::vec2 {
                return static_cast<glm::vec2>(input);
            }
        );
        return floats;
    }
    else {
        return std::get<std::vector<glm::vec2>>(property);
    }
}

[[nodiscard]] const NodeProperty& NodeProperty::get_subproperty(std::string_view subproperty_name) const {
    const auto& vector = get_subproperties();
    auto iter = std::find_if(
        vector.begin(), vector.end(),
        [&subproperty_name] (const NodeProperty& prop) -> bool {
            return prop.get_name() == subproperty_name;
        }
    );

    if (iter == vector.end()) {
        throw std::runtime_error(fmt::format(
            "Can't find a subproperty with the specified name: \"{}\".", subproperty_name
        ));
    }

    return *iter;
}

[[nodiscard]] const NodeProperty* NodeProperty::get_optional_subproperty(std::string_view subproperty_name) const {
    const auto& vector = get_subproperties();
    auto iter = std::find_if(
        vector.begin(), vector.end(),
        [&subproperty_name] (const NodeProperty& prop) -> bool {
            return prop.get_name() == subproperty_name;
        }
    );

    if (iter == vector.end()) {
        return nullptr;
    }

    return &*iter;
}