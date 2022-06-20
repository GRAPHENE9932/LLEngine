#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <span>

#include "../consts_and_enums.hpp"
#include "Texture.hpp"

class BitmapFont {
public:
    BitmapFont(const std::string& file_name);

    GLuint get_texture_id() const;
    glm::vec2 get_char_size() const;
    glm::vec2 get_spacing() const;
    void get_uvs_for_char(const char glyph, std::span<float, QUAD_UVS.size()>& uvs_out) const;

private:
    glm::u32vec2 char_size;
    glm::u32vec2 spacing;
    glm::u32vec2 tex_size;
    std::unordered_map<char, uint32_t> char_to_index;

    std::shared_ptr<Texture> texture;
};
