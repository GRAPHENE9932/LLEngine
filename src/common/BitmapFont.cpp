#include <fstream>
#include <stdexcept>
#include <limits>

#include "consts_and_enums.hpp"
#include "KTXTexture.hpp"
#include "BitmapFont.hpp"

BitmapFont::BitmapFont(const std::string& file_name) {
    std::ifstream stream(file_name);
    if (!stream)
        throw std::runtime_error("Failed to load the bitmap font. File name: " + file_name);
    
    //  LLBMF specification:
    //  The first line is header.
    //      Format: "<WIDTH> <HEIGHT> <SPACING_X> <SPACING_Y> <TEX_FILE> <TEX_WIDTH> <TEX_HEIGHT>"
    //      All properties, except <TEX_FILE> are 32-bit unsigned integers
    //      and <TEX_FILE> is string that contains the texture image file name.
    //      <WIDTH> and <HEIGHT> means every character's file width and height.
    //      <SPACING_X> and <SPACING_Y> means spacing between characters. Both
    //      on drawing and in texture.
    //      <TEX_WIDTH> and <TEX_HEIGHT> is image width and height of the texture image.
    //  The second line is data.
    //      This line contains continuous array of ASCII characters in the same
    //      sequence as in the texture file.
    //  No '\n' should left as the last character.
    std::string tex_file_name;
    stream >> char_size.x >> char_size.y >>
            spacing.x >> spacing.y >>
            tex_file_name >> tex_size.x >> tex_size.y;
    if (!stream)
        throw std::runtime_error("Failed to parse the bitmap font:\n"
                "invalid header data\n"
                "File name: " + file_name);
    
    // Skip to the next line.
    if (!stream.ignore(std::numeric_limits<std::streamsize>::max(), '\n'))
        throw std::runtime_error("Failed to parse the bitmap font:\n"
                "looks like there is no second line in this file.\n"
                "File name: " + file_name);

    // Read charecter by character up to \n or EOF.
    // Yes, I could use the std::getline function, but
    // it skips a whitespace at the end (if present).
    char cur_char {};
    uint32_t char_index {0};
    stream >> std::noskipws >> cur_char;
    while (stream && cur_char != '\n') {
        char_to_index[cur_char] = char_index++;
        stream >> std::noskipws >> cur_char;
    }
    
    // Load texture file.
    texture = std::make_shared<KTXTexture>(tex_file_name);
}

GLuint BitmapFont::get_texture_id() const {
    return texture->get_id();
}

glm::vec2 BitmapFont::get_char_size() const {
    return static_cast<glm::vec2>(char_size);
}

glm::vec2 BitmapFont::get_spacing() const {
    return static_cast<glm::vec2>(spacing);
}

inline glm::vec2 vec2_max(const glm::vec2& vec_1, const glm::vec2& vec_2) {
    return {std::max(vec_1.x, vec_2.x), std::max(vec_1.y, vec_2.y)};
}

void BitmapFont::get_uvs_for_char(const char glyph, std::span<float, QUAD_UVS.size()>& uvs_out) const {
    const auto index_iter = char_to_index.find(glyph);
    if (index_iter == char_to_index.end()) {
        if (char_to_index.contains('?')) {
            get_uvs_for_char('?', uvs_out);
            return;
        }
        else {
            throw std::runtime_error(std::string("The glyph \'") + glyph + "\' does not exist");
        }
    }
    const std::size_t index {(*index_iter).second};

    const glm::vec2 char_uv_size = static_cast<glm::vec2>(get_char_size()) / static_cast<glm::vec2>(tex_size);
    const glm::vec2 spacing_uv = static_cast<glm::vec2>(get_spacing()) / static_cast<glm::vec2>(tex_size);

    const glm::u32vec2 resolution_in_chars {
        (tex_size - char_size) / (char_size + spacing) + glm::u32vec2(1, 1)
    };
    const glm::u32vec2 char_row_and_col {
        index % resolution_in_chars.x,
        index / resolution_in_chars.x
    };
    
    const glm::vec2 char_uv_pos {
        static_cast<glm::vec2>(char_row_and_col) * (char_uv_size + spacing_uv)
    };

    std::copy(QUAD_UVS.begin(), QUAD_UVS.end(), uvs_out.begin());
    for (std::size_t i = 0; i < QUAD_UVS.size(); i += 2) {
        uvs_out[i] *= char_uv_size.x;
        uvs_out[i + 1] *= char_uv_size.y;

        uvs_out[i] += char_uv_pos.x;
        uvs_out[i + 1] += char_uv_pos.y;
    }
}
