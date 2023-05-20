#pragma once

#include "rendering/Texture.hpp"

#include <glm/vec2.hpp>

#include <map>
#include <string>

namespace llengine {
class FontError : public std::runtime_error {
public:
    FontError(const std::string& message) : std::runtime_error(message) {}
};

class FreeTypeFont {
public:
    struct FontChar {
        Texture texture;
        glm::u32vec2 position;
        std::uint32_t advance;
    };

    FreeTypeFont(const std::string& file_path, std::uint32_t font_size);

    const FontChar& get_font_char(char character) const {
        return chars.at(character);
    }

private:
    std::map<char, FontChar> chars;
};
}