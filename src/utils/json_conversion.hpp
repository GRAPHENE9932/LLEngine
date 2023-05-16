#pragma once

#include <glm/vec2.hpp> // glm::vec2
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/quaternion.hpp> // glm::quat
#include <nlohmann/json.hpp> // nlohmann::json

#include <optional>

template<typename T>
std::optional<T> get_optional(const nlohmann::json& upper_json, const std::string& name) {
    if (upper_json.contains(name))
        return std::make_optional<T>(upper_json.at(name).get<T>());
    else
        return std::nullopt;
}

template<typename T>
T get_optional(const nlohmann::json& upper_json, std::string_view name, const T& default_value) {
    if (upper_json.contains(name))
        return upper_json.at(name).get<T>();
    else
        return default_value;
}

namespace glm {
    inline void from_json(const nlohmann::json& root_json, glm::vec2& vec) {
        vec = {
            root_json.at(0).get<float>(),
            root_json.at(1).get<float>()
        };
    }

    inline void from_json(const nlohmann::json& root_json, glm::vec3& vec) {
        vec = {
            root_json.at(0).get<float>(),
            root_json.at(1).get<float>(),
            root_json.at(2).get<float>()
        };
    }

    inline void from_json(const nlohmann::json& root_json, glm::vec4& vec) {
        vec = {
            root_json.at(0).get<float>(),
            root_json.at(1).get<float>(),
            root_json.at(2).get<float>(),
            root_json.at(3).get<float>()
        };
    }

    inline void from_json(const nlohmann::json& root_json, glm::quat& vec) {
        vec = {
            root_json.at(3).get<float>(),
            root_json.at(0).get<float>(),
            root_json.at(1).get<float>(),
            root_json.at(2).get<float>()
        };
    }

    inline void from_json(const nlohmann::json& j, glm::mat4& mat) {
        mat = {
            {j.at(0), j.at(1), j.at(2), j.at(3)},
            {j.at(4), j.at(5), j.at(6), j.at(7)},
            {j.at(8), j.at(9), j.at(10), j.at(11)},
            {j.at(12), j.at(13), j.at(14), j.at(15)}
        };
    }
}
