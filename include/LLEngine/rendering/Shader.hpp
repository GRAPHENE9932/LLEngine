#pragma once

#include "datatypes.hpp"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstring>
#include <string_view>
#include <stdexcept>
#include <type_traits>
#include <vector>
#include <concepts>
#include <functional>

namespace llengine {
namespace internal {
void set_int(ShaderID shader_id, ShaderUniformID uniform_id, std::int32_t value);
void set_float(ShaderID shader_id, ShaderUniformID uniform_id, float value);
void set_vec2(ShaderID shader_id, ShaderUniformID uniform_id, glm::vec2 value);
void set_vec3(ShaderID shader_id, ShaderUniformID uniform_id, const glm::vec3& value);
void set_vec4(ShaderID shader_id, ShaderUniformID uniform_id, const glm::vec4& value);
void set_mat3(ShaderID shader_id, ShaderUniformID uniform_id, const glm::mat3& value);
void set_mat4(ShaderID shader_id, ShaderUniformID uniform_id, const glm::mat4& value);

void load_shader(
    std::string_view vertex_shader_code, std::string_view fragment_shader_code,
    const std::vector<std::string>& defines, ShaderID& id_out
);
ShaderUniformID query_uniform_id(ShaderID shader_id, std::string_view name);

template<std::size_t N>
[[nodiscard]] constexpr bool compare_strings(const char(& str_left)[N], const char(& str_right)[N]) {
    for (std::size_t i = 0; i < N; i++) {
        if (str_left[i] != str_right[i]) {
            return false;
        }
    }

    return true;
}
}

template<size_t N>
struct ConstUniformName {
    constexpr ConstUniformName(const char(& str)[N]) {
        std::copy_n(str, N, value);
    }
    
    template<size_t OtherN>
    constexpr bool operator==(const ConstUniformName<OtherN>& other) const {
        if constexpr (N != OtherN) {
            return false;
        }
        else {
            return internal::compare_strings<N>(value, other.value);
        }
    }

    char value[N];
};

template<ConstUniformName... UniformNames>
class Shader {
public:
    Shader(
        std::string_view vertex_shader_code, std::string_view fragment_shader_code,
        const std::vector<std::string>& defines = {}
    ) {
        internal::load_shader(vertex_shader_code, fragment_shader_code, defines, shader_id);

        std::size_t index {0};
        (query_uniform_id_and_increment_index<UniformNames>(index), ...);
    }

    template<ConstUniformName Name>
    void set_int(std::int32_t value) const {
        internal::set_int(shader_id, get_id_from_name<Name>(), value);
    }

    template<ConstUniformName Name>
    void set_float(float value) const {
        internal::set_float(shader_id, get_id_from_name<Name>(), value);
    }

    template<ConstUniformName Name>
    void set_vec2(glm::vec2 value) const {
        internal::set_vec2(shader_id, get_id_from_name<Name>(), value);
    }

    template<ConstUniformName Name>
    void set_vec3(const glm::vec3& value) const {
        internal::set_vec3(shader_id, get_id_from_name<Name>(), value);
    }

    template<ConstUniformName Name>
    void set_vec4(const glm::vec4& value) const {
        internal::set_vec4(shader_id, get_id_from_name<Name>(), value);
    }

    template<ConstUniformName Name>
    void set_mat3(const glm::mat3& value) const {
        internal::set_mat3(shader_id, get_id_from_name<Name>(), value);
    }

    template<ConstUniformName Name>
    void set_mat4(const glm::mat4& value) const {
        internal::set_mat4(shader_id, get_id_from_name<Name>(), value);
    }

    template<ConstUniformName Name, typename T>
    void set_uniform(const T& value) {
        if constexpr (std::is_same_v<T, std::int32_t>) {
            set_int<Name>(value);
        }
        else if constexpr (std::is_same_v<T, float>) {
            set_float<Name>(value);
        }
        else if constexpr (std::is_same_v<T, glm::vec2>) {
            set_vec2<Name>(value);
        }
        else if constexpr (std::is_same_v<T, glm::vec3>) {
            set_vec3<Name>(value);
        }
        else if constexpr (std::is_same_v<T, glm::vec4>) {
            set_vec4<Name>(value);
        }
        else if constexpr (std::is_same_v<T, glm::mat3>) {
            set_mat3<Name>(value);
        }
        else if constexpr (std::is_same_v<T, glm::mat4>) {
            set_mat4<Name>(value);
        }
        else {
            static_assert(true, "Unknown uniform variable type.");
        }
    }

    template<ConstUniformName Name>
    ShaderUniformID get_id_from_name() const {
        return uniform_ids.at(get_array_index_from_name<Name>());
    }

private:
    ShaderID shader_id;
    std::array<ShaderUniformID, sizeof...(UniformNames)> uniform_ids;

    template<ConstUniformName Name>
    constexpr std::size_t get_array_index_from_name() const {
        std::size_t index {0};
        increment_index_until_found<Name, UniformNames...>(index);
        return index;
    }

    template<ConstUniformName Name>
    constexpr void query_uniform_id_and_increment_index(std::size_t& index) {
        uniform_ids[index] = internal::query_uniform_id(shader_id, Name.value);
        index++;
    }

    template<ConstUniformName Searched, ConstUniformName First, ConstUniformName... Last>
    static constexpr void increment_index_until_found(std::size_t& current_index) {
        if constexpr (First != Searched) {
            increment_index_until_found<Searched, Last...>(++current_index);
        }
    }
};
}