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
#include <memory>
#include <string_view>
#include <stdexcept>
#include <type_traits>
#include <vector>
#include <concepts>
#include <functional>
#include <string>

namespace llengine {
namespace internal {
void use_shader(ShaderID shader_id);
void set_int(ShaderID shader_id, ShaderUniformID uniform_id, std::int32_t value);
void set_float(ShaderID shader_id, ShaderUniformID uniform_id, float value);
void set_vec2(ShaderID shader_id, ShaderUniformID uniform_id, glm::vec2 value);
void set_vec3(ShaderID shader_id, ShaderUniformID uniform_id, const glm::vec3& value);
void set_vec4(ShaderID shader_id, ShaderUniformID uniform_id, const glm::vec4& value);
void set_mat3(ShaderID shader_id, ShaderUniformID uniform_id, const glm::mat3& value);
void set_mat4(ShaderID shader_id, ShaderUniformID uniform_id, const glm::mat4& value);
void bind_2d_texture(ShaderUniformID uniform_id, GraphicsAPIEnum unit, TextureID texture_id);
void bind_cubemap_texture(ShaderUniformID uniform_id, GraphicsAPIEnum unit, TextureID texture_id);

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
        (query_primitive_uniform_id_and_increment_index<UniformNames>(index), ...);
    }

    template<ConstUniformName Name>
    void set_int(std::int32_t value) const {
        internal::set_int(shader_id, get_uniform_id<Name>(), value);
    }

    template<ConstUniformName Name>
    void set_int(std::size_t index, std::int32_t value) const {
        internal::set_int(shader_id, get_uniform_id<Name>(index), value);
    }

    template<ConstUniformName Name>
    void set_float(float value) const {
        internal::set_float(shader_id, get_uniform_id<Name>(), value);
    }

    template<ConstUniformName Name>
    void set_float(std::size_t index, float value) const {
        internal::set_float(shader_id, get_uniform_id<Name>(index), value);
    }

    template<ConstUniformName Name>
    void set_vec2(glm::vec2 value) const {
        internal::set_vec2(shader_id, get_uniform_id<Name>(), value);
    }

    template<ConstUniformName Name>
    void set_vec2(std::size_t index, glm::vec2 value) const {
        internal::set_vec2(shader_id, get_uniform_id<Name>(index), value);
    }

    template<ConstUniformName Name>
    void set_vec3(const glm::vec3& value) const {
        internal::set_vec3(shader_id, get_uniform_id<Name>(), value);
    }

    template<ConstUniformName Name>
    void set_vec3(std::size_t index, const glm::vec3& value) const {
        internal::set_vec3(shader_id, get_uniform_id<Name>(index), value);
    }

    template<ConstUniformName Name>
    void set_vec4(const glm::vec4& value) const {
        internal::set_vec4(shader_id, get_uniform_id<Name>(), value);
    }

    template<ConstUniformName Name>
    void set_vec4(std::size_t index, const glm::vec4& value) const {
        internal::set_vec4(shader_id, get_uniform_id<Name>(index), value);
    }

    template<ConstUniformName Name>
    void set_mat3(const glm::mat3& value) const {
        internal::set_mat3(shader_id, get_uniform_id<Name>(), value);
    }

    template<ConstUniformName Name>
    void set_mat3(std::size_t index, const glm::mat3& value) const {
        internal::set_mat3(shader_id, get_uniform_id<Name>(index), value);
    }

    template<ConstUniformName Name>
    void set_mat4(const glm::mat4& value) const {
        internal::set_mat4(shader_id, get_uniform_id<Name>(), value);
    }

    template<ConstUniformName Name>
    void set_mat4(std::size_t index, const glm::mat4& value) const {
        internal::set_mat4(shader_id, get_uniform_id<Name>(index), value);
    }

    template<ConstUniformName Name>
    void bind_2d_texture(TextureID texture_id, GraphicsAPIEnum texture_unit) const {
        internal::bind_2d_texture(get_uniform_id<Name>(), texture_unit, texture_id);
    }

    template<ConstUniformName Name>
    void bind_cubemap_texture(TextureID texture_id, GraphicsAPIEnum texture_unit) const {
        internal::bind_cubemap_texture(get_uniform_id<Name>(), texture_unit, texture_id);
    }

    void use_shader() const {
        internal::use_shader(shader_id);
    }

    template<ConstUniformName Name>
    [[nodiscard]] constexpr ShaderUniformID get_uniform_id() const {
        return primitive_uniform_ids[get_index_of_primitive<Name>()];
    }

    template<ConstUniformName Name>
    [[nodiscard]] constexpr ShaderUniformID get_uniform_id(std::size_t index) const {
        if (arrays[get_index_of_array<Name>()].at(index) == -1) {
            assign_uniform_id_to_array_element<Name>(index);
        }
        return arrays[get_index_of_array<Name>()].at(index);
    }

    template<ConstUniformName Name>
    [[nodiscard]] constexpr bool is_uniform_initialized() const {
        return get_uniform_id<Name>() != -1;
    }

    [[nodiscard]] ShaderID get_program_id() const {
        return shader_id;
    }

private:
    ShaderID shader_id;

    template<ConstUniformName Name>
    [[nodiscard]] static constexpr std::size_t is_primitive() {
        std::string name_str = Name.value;
        return name_str.find("[]") == std::string::npos;
    }

    template<ConstUniformName Name>
    static constexpr void increment_index_if_primitive(std::size_t& index) {
        if constexpr (is_primitive<Name>()) {
            index++;
        }
    }

    [[nodiscard]] static constexpr std::size_t get_amount_of_primitive_uniforms() {
        std::size_t index {0};
        (increment_index_if_primitive<UniformNames>(index), ...);
        return index;
    }

    std::array<ShaderUniformID, get_amount_of_primitive_uniforms()> primitive_uniform_ids;

    template<ConstUniformName Name>
    [[nodiscard]] static constexpr std::size_t is_array() {
        std::string name_str = Name.value;
        return name_str.find("[]") != std::string::npos;
    }

    template<ConstUniformName Name>
    static constexpr void increment_index_if_array(std::size_t& index) {
        if constexpr (is_array<Name>()) {
            index++;
        }
    }

    [[nodiscard]] static constexpr std::size_t get_amount_of_arrays() {
        std::size_t index {0};
        (increment_index_if_array<UniformNames>(index), ...);
        return index;
    }

    mutable std::array<std::vector<ShaderUniformID>, get_amount_of_arrays()> arrays;

    void ensure_array_has_size_of(std::size_t array_index, std::size_t size) const {
        if (arrays.at(array_index).size() < size) {
            arrays[array_index].resize(size, -1);
        }
    }

    template<ConstUniformName Name>
    void assign_uniform_id_to_array_element(std::size_t element_index) const {
        ensure_array_has_size_of(get_index_of_array<Name>(), element_index + 1);

        arrays.at(get_index_of_array<Name>()).at(element_index) =
            internal::query_uniform_id(shader_id, insert_index_in_brackets(Name.value, element_index));
    }

    template<ConstUniformName Name>
    constexpr std::size_t get_index_of_primitive() const {
        std::size_t index {0};
        search_primitive<Name, UniformNames...>(index);
        return index;
    }

    template<ConstUniformName Searched, ConstUniformName First, ConstUniformName... Last>
    static constexpr void search_primitive(std::size_t& current_index) {
        if constexpr (First != Searched) {
            if constexpr (is_primitive<First>()) {
                current_index++;
            }
            search_primitive<Searched, Last...>(current_index);
        }
    }

    template<ConstUniformName Name>
    constexpr std::size_t get_index_of_array() const {
        std::size_t index {0};
        search_array<Name, UniformNames...>(index);
        return index;
    }

    template<ConstUniformName Searched, ConstUniformName First, ConstUniformName... Last>
    static constexpr void search_array(std::size_t& current_index) {
        if constexpr (First != Searched) {
            if constexpr (is_array<First>()) {
                current_index++;
            }
            search_array<Searched, Last...>(current_index);
        }
    }

    static constexpr std::string insert_index_in_brackets(std::string name, std::size_t index) {
        std::size_t bracket_position {name.find('[')};
        name.insert(bracket_position + 1, std::to_string(index));
        return name;
    }

    template<ConstUniformName Name>
    constexpr void query_primitive_uniform_id_and_increment_index(std::size_t& index) {
        if (!is_primitive<Name>()) {
            index++;
            return;
        }

        primitive_uniform_ids[index++] = internal::query_uniform_id(shader_id, Name.value);
    }
};
}