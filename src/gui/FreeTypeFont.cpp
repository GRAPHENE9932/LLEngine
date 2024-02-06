#include "gui/FreeTypeFont.hpp"
#include "NodeProperty.hpp"

#include <fmt/format.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <GL/glew.h>

#include <optional>
#include <stdexcept>

using namespace llengine;

FT_Library ft_lib;
bool is_ft_lib_initialized = false;

void destroy_freetype() {
    if (!is_ft_lib_initialized) {
        return;
    }

    const auto done_result {FT_Done_FreeType(ft_lib)};
    if (done_result != 0) {
        throw std::runtime_error(fmt::format(
            "Failed to destroy the FreeType library. Error code: {}", done_result
        ));
    }
}

void initialize_freetype() {
    const auto init_result {FT_Init_FreeType(&ft_lib)};
    if (init_result != 0) {
        throw std::runtime_error(fmt::format(
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

static bool read_bit(unsigned char* memory, std::size_t index) {
    std::size_t byte_offset = index / 8;
    std::size_t bit_offset = index % 8;
    unsigned char byte = memory[byte_offset];
    return static_cast<bool>((0b10000000 >> bit_offset) & byte);
}

static void read_mono_bitmap(const FT_Bitmap& bitmap, unsigned char* output) {
    assert(bitmap.pixel_mode == FT_Pixel_Mode::FT_PIXEL_MODE_MONO);

    std::size_t offset = 0;
    for (std::size_t row = 0; row < bitmap.rows; row++) {
        for (std::size_t col = 0; col < bitmap.width; col++) {
            output[row * bitmap.width + col] = read_bit(bitmap.buffer + offset, col) ? 255 : 0;
        }

        offset += bitmap.pitch;
    }
}

static void read_gray_bitmap(const FT_Bitmap& bitmap, unsigned char* output) {
    assert(bitmap.pixel_mode == FT_Pixel_Mode::FT_PIXEL_MODE_GRAY);

    std::size_t bitmap_offset = 0;
    std::size_t output_offset = 0;
    for (std::size_t row = 0; row < bitmap.rows; row++) {
        std::memcpy(output + output_offset, bitmap.buffer + bitmap_offset, bitmap.width);
        bitmap_offset += bitmap.pitch;
        output_offset += bitmap.width;
    }
}

static Texture texture_from_freetype_bitmap(const FT_Bitmap& bitmap) {
    const std::size_t buffer_size = bitmap.width * bitmap.rows;
    std::vector<unsigned char> buffer(buffer_size);
    
    if (bitmap.pixel_mode == FT_Pixel_Mode::FT_PIXEL_MODE_MONO) {
        read_mono_bitmap(bitmap, buffer.data());
    }
    else if (bitmap.pixel_mode == FT_Pixel_Mode::FT_PIXEL_MODE_GRAY) {
        read_gray_bitmap(bitmap, buffer.data());
    }
    else {
        throw std::runtime_error("Unsupported FreeType bitmap pixel mode.");
    }

    const glm::u32vec2 tex_size {
        bitmap.width,
        bitmap.rows
    };

    ManagedTextureID texture_id;
    glGenTextures(1, &texture_id.get_ref());
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RED, tex_size.x, tex_size.y, 0,
        GL_RED, GL_UNSIGNED_BYTE, buffer.data()
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return Texture(std::move(texture_id), tex_size, Texture::Type::TEX_2D);
}

FreeTypeFont::FreeTypeFont(const std::string& file_path, std::uint32_t font_size) : font_size(font_size) {
    if (!is_ft_lib_initialized) {
        initialize_freetype();
    }

    ManagedFTFace face;
    FT_Error error {FT_New_Face(ft_lib, file_path.c_str(), 0, &face)};
    if (error != 0) {
        throw std::runtime_error(fmt::format(
            "Failed to initialize the FreeType face from file \"{}\". Error code: {}",
            file_path, error
        ));
    }
    error = FT_Set_Pixel_Sizes(face, 0, font_size);
    if (error != 0) {
        throw std::runtime_error(fmt::format(
            "Failed to set FreeType font size. File path: \"{}\", error code: {}, font size: {}.",
            file_path, error, font_size
        ));
    }

    // Load all the characters.
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    for (char c = 0; c < 127; c++) {
        error = FT_Load_Char(face, c, FT_LOAD_RENDER);
        if (error != 0) {
            throw std::runtime_error(fmt::format(
                "Failed to load character {:x}. Font path: \"{}\", error code: {}.",
                c, file_path, error
            ));
        }
        
        chars.emplace(
            c,
            FontChar {
                texture_from_freetype_bitmap(face->glyph->bitmap),
                glm::u32vec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                static_cast<std::uint32_t>(face->glyph->advance.x)
            }
        );
    }
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4); // Return to the default value.
}

std::shared_ptr<FreeTypeFont> FreeTypeFont::from_property(const NodeProperty& property) {
    std::string type = property.get_subproperty("type").get<std::string>();
    if (type != "font") {
        throw std::runtime_error(fmt::format(
            "Failed to convert node property into FreeType font: invalid type: \"{}\".",
            type
        ));
    }

    std::string file_path = property.get_subproperty("path").get<std::string>();
    std::uint32_t font_size = property.get_subproperty("size").get<std::uint32_t>();

    return std::make_shared<FreeTypeFont>(file_path, font_size);
}