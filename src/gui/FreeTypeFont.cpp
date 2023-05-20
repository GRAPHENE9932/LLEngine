#include "gui/FreeTypeFont.hpp"

#include <fmt/format.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include <optional>

using namespace llengine;

FT_Library ft_lib;
bool is_ft_lib_initialized = false;

void destroy_freetype() {
    if (!is_ft_lib_initialized) {
        return;
    }

    const auto done_result {FT_Done_FreeType(ft_lib)};
    if (done_result != 0) {
        throw FontError(fmt::format(
            "Failed to destroy the FreeType library. Error code: {}", done_result
        ));
    }
}

void initialize_freetype() {
    const auto init_result {FT_Init_FreeType(&ft_lib)};
    if (init_result != 0) {
        throw FontError(fmt::format(
            "Failed to initialize the FreeType library. Error code: {}", init_result
        ));
    }

    is_ft_lib_initialized = true;
    std::atexit(destroy_freetype);
}

class ManagedFTFace {
public:
    ManagedFTFace() = default;
    ManagedFTFace(const ManagedFTFace& other) = delete;
    ManagedFTFace(ManagedFTFace&& other) = delete;

    ManagedFTFace& operator=(const ManagedFTFace& other) = delete;

    inline FT_Face& operator->() noexcept {
        return face;
    }

    inline FT_Face* operator&() noexcept {
        return &face;
    }

    inline operator FT_Face&() noexcept {
        return face;
    }

    ~ManagedFTFace() {
        FT_Done_Face(face);
    }

private:
    FT_Face face;
};

FreeTypeFont::FreeTypeFont(const std::string& file_path, std::uint32_t font_size) {
    if (!is_ft_lib_initialized) {
        initialize_freetype();
    }

    ManagedFTFace face;
    FT_Error error {FT_New_Face(ft_lib, file_path.c_str(), 0, &face)};
    if (error != 0) {
        throw FontError(fmt::format(
            "Failed to initialize the FreeType face from file \"{}\". Error code: {}",
            file_path, error
        ));
    }
    error = FT_Set_Pixel_Sizes(face, 0, font_size);
    if (error != 0) {
        throw FontError(fmt::format(
            "Failed to set FreeType font size. File path: \"{}\", error code: {}, font size: {}.",
            file_path, error, font_size
        ));
    }

    // Load all the characters.
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    for (char c = 0; c < 127; c++) {
        error = FT_Load_Char(face, c, FT_LOAD_RENDER);
        if (error != 0) {
            throw FontError(fmt::format(
                "Failed to load character {:x}. Font path: \"{}\", error code: {}.",
                c, file_path, error
            ));
        }

        const glm::u32vec2 tex_size {
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows
        };

        GLuint texture_id {0};
        glGenTextures(1, &texture_id);
        glBindTexture(GL_TEXTURE_2D, texture_id);
        glTexImage2D(
            GL_TEXTURE_2D, 0, GL_RED, tex_size.x, tex_size.y, 0,
            GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer
        );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        chars.emplace(
            c,
            FontChar {
                Texture(texture_id, tex_size, false),
                glm::u32vec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                static_cast<std::uint32_t>(face->glyph->advance.x)
            }
        );
    }
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4); // Return to the default value.
}