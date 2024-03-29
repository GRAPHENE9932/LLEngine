#pragma once

#include "rendering/Texture.hpp"

#include <glm/vec2.hpp>

#include <map>
#include <string>
#include <memory>

namespace llengine {
class FreeTypeFont {
public:
    struct FontChar {
        Texture texture;
        glm::u32vec2 position;
        std::uint32_t advance;
        float texture_scale;
    };

    FreeTypeFont(const std::string& file_path, std::uint32_t font_size);

    const FontChar& get_font_char(char character) const {
        return chars.at(character);
    }

    [[nodiscard]] inline std::uint32_t get_font_size() const {
        return scaled_size;
    }

    static std::shared_ptr<FreeTypeFont> from_property(const NodeProperty& property);

private:
    std::map<char, FontChar> chars;
    std::uint32_t scaled_size;
    std::uint32_t unscaled_size;
};
}