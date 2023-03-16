#include "RGBETexture.hpp"

#include <cmath>
#include <tuple>
#include <array>
#include <memory>
#include <vector>
#include <fstream>
#include <sstream>
#include <optional>
#include <algorithm>
#include <stdexcept>

constexpr std::string_view RGBE_IDENTIFIER {"#?RADIANCE"};

class RGBELoadingError : public std::runtime_error {
public:
    RGBELoadingError(std::string_view message, std::string_view file_path) :
            std::runtime_error(construct_full_message(message, file_path)) {}

private:
    std::string
    construct_full_message(std::string_view message, std::string_view file_path) {
        return "Failed to load the RGBE file. " + std::string(message) +
                "\n File path: \"" + std::string(file_path) + '\"';
    }
};

class RGBEDataLoadingError : public std::runtime_error {
public:
    RGBEDataLoadingError(const std::string& message) :
            std::runtime_error(message) {} 
};

template<typename InputIter, typename OutputIter>
void rgbe_to_rgb(
    InputIter rgbe_data_begin, InputIter rgbe_data_end,
    OutputIter rgb_data_begin
) {
    static_assert(std::is_same<typename std::remove_reference<decltype(*rgbe_data_begin)>::type, uint8_t>());
    static_assert(std::is_same<typename std::remove_reference<decltype(*rgbe_data_end)>::type, uint8_t>());
    static_assert(std::is_same<typename std::remove_reference<decltype(*rgb_data_begin)>::type, float>());
    assert((rgbe_data_end - rgbe_data_begin) % 4 == 0);

    const auto size {(rgbe_data_end - rgbe_data_begin) / 4};
    for (std::size_t i = 0; i < size; i++) {
        float exponent = std::ldexp(1.0f, *(rgbe_data_begin + size * 3 + i) - 128);
        *rgb_data_begin++ = *(rgbe_data_begin + size * 0 + i) / 255.0f * exponent;
        *rgb_data_begin++ = *(rgbe_data_begin + size * 1 + i) / 255.0f * exponent;
        *rgb_data_begin++ = *(rgbe_data_begin + size * 2 + i) / 255.0f * exponent;
    }
}

std::vector<float> read_run_length_encoded_data(
    std::uint32_t width,
    std::uint32_t height,
    std::ifstream& stream
) {
    std::vector<float> result(width * height * 3);

    for (std::uint32_t scanline_i = 0; scanline_i < height; scanline_i++) {
        std::array<std::uint8_t, 4> first_4_bytes;
        stream.read(reinterpret_cast<char*>(first_4_bytes.data()), first_4_bytes.size());
        std::uint16_t scanline_width = first_4_bytes[2] << 8 | first_4_bytes[3];
        if (scanline_width != width) {
            throw RGBEDataLoadingError("Ambiguous texture width.");
        }

        std::vector<std::uint8_t> scanline_buffer(width * 4);

        for (std::uint32_t cur_column = 0; cur_column < width * 4;) {
            std::uint8_t run_length;
            stream.read(reinterpret_cast<char*>(&run_length), 1);

            if (run_length > 128) {
                // A run of the same value.
                run_length -= 128;

                if (run_length == 0 || scanline_buffer.size() < cur_column + run_length) {
                    throw RGBEDataLoadingError("Invalid run length in run-length encoded data.");
                }

                std::uint8_t value;
                stream.read(reinterpret_cast<char*>(&value), 1);
                std::fill(
                    scanline_buffer.begin() + cur_column,
                    scanline_buffer.begin() + cur_column + run_length,
                    value
                );

                cur_column += run_length;
            }
            else {
                // A run of different values.
                if (run_length == 0 || scanline_buffer.size() < cur_column + run_length) {
                    throw RGBEDataLoadingError("Invalid run length in run-length encoded data.");
                }

                stream.read(
                    reinterpret_cast<char*>(scanline_buffer.data() + cur_column),
                    run_length
                );

                cur_column += run_length;
            }
        }

        if (!stream) {
            throw RGBEDataLoadingError("Failed to read a scanline.");
        }

        rgbe_to_rgb(
            scanline_buffer.begin(), scanline_buffer.end(),
            result.begin() + width * 3 * scanline_i
        );
    }

    return result;
}

std::vector<float> get_rgb_data(
    std::uint32_t width,
    std::uint32_t height,
    std::ifstream& stream
) {
    std::array<std::uint8_t, 4> first_4_bytes;
    stream.read(reinterpret_cast<char*>(first_4_bytes.data()), first_4_bytes.size());
    std::uint16_t scanline_width = first_4_bytes[2] << 8 | first_4_bytes[3];

    // Get back for 4 bytes we just read.
    stream.seekg(static_cast<std::size_t>(stream.tellg()) - 4);
    
    if (first_4_bytes[0] == 2 && first_4_bytes[1] == 2 && scanline_width < 32768) {
        // Data is run-length encoded.
        return read_run_length_encoded_data(width, height, stream);
    }
    else {
        std::vector<std::uint8_t> rgbe_data(width * height * 4);
        stream.read(reinterpret_cast<char*>(rgbe_data.data()), rgbe_data.size());

        if (!stream) {
            throw RGBEDataLoadingError("Failed to read flat data.");
        }

        std::vector<float> result(width * height * 3);
        rgbe_to_rgb(rgbe_data.begin(), rgbe_data.end(), result.begin());
        return result;
    }
}

GLuint initialize_opengl_texture(
    std::uint32_t width,
    std::uint32_t height,
    const Texture::Parameters& params,
    const std::vector<float>& rgb_data
) {
    GLuint texture_id = 0;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glTexStorage2D(
        GL_TEXTURE_2D, 1, GL_RGB32F,
        width, height
    );
    glTexSubImage2D(
        GL_TEXTURE_2D, 0, 0, 0, width, height,
        GL_RGB, GL_FLOAT, rgb_data.data()
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, params.magnification_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, params.minification_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, params.wrap_s);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, params.wrap_t);

    return texture_id;
}

std::optional<std::pair<std::string_view, std::string_view>>
parse_header_variable(std::string_view line_with_variable) {
    std::size_t equal_sign_index = line_with_variable.find('=');
    if (equal_sign_index == std::string_view::npos) {
        return std::nullopt;
    }

    return {{
        std::string_view(
            line_with_variable.begin(),
            equal_sign_index
        ),
        std::string_view(
            line_with_variable.begin() + equal_sign_index + 1,
            line_with_variable.end()
        )
    }};
}

RGBETexture::RGBETexture(const Parameters& params) {
    std::ifstream stream(params.file_path, std::ios::binary | std::ios::in);
    if (!stream) {
        throw RGBELoadingError("Failed to open the file.", params.file_path);
    }
    stream.ignore(params.offset);

    // Check identifier.
    std::string identifier;
    std::getline(stream, identifier);
    if (identifier != RGBE_IDENTIFIER) {
        throw RGBELoadingError("Invalid identifier (magic bytes).", params.file_path);
    }

    // Handle variables.
    bool has_valid_format = false;

    std::string current_line;
    std::getline(stream, current_line);
    while (!current_line.empty()) {
        const auto variable = parse_header_variable(current_line);
        if (!variable.has_value()) {
            throw RGBELoadingError("Failed to parse a line with variable.", params.file_path);
        }

        if (variable->first == "FORMAT") {
            has_valid_format = variable->second == "32-bit_rle_rgbe";
        }
        // COLORCORR, EXPOSURE, PIXASPECT, VIEW, PRIMARIES are ignored.

        std::getline(stream, current_line);
    }

    if (!has_valid_format) {
        throw RGBELoadingError("Invalid FORMAT. Only 32-bit_rle_rgbe is supported", params.file_path);
    }

    // Handle the resolution string.
    std::string resolution_string;
    std::getline(stream, resolution_string);
    std::stringstream resolution_ss(resolution_string);

    std::string x_orientation, y_orientation;
    std::uint32_t x_resolution, y_resolution;

    try {
        resolution_ss >> y_orientation >> y_resolution >> x_orientation >> x_resolution;
    }
    catch (...) {
        throw RGBELoadingError("Invalid resolution string.", params.file_path);
    }

    if (x_resolution < 1 || y_resolution < 1) {
        throw RGBELoadingError("Invalid resolution.", params.file_path);
    }

    if (x_orientation != "+X" || y_orientation != "-Y") {
        throw RGBELoadingError("Invalid resolution string. Only +X -Y resolution supported.", params.file_path);
    }

    try {
        std::vector<float> rgb_data {get_rgb_data(x_resolution, y_resolution, stream)};
        this->texture_id = initialize_opengl_texture(x_resolution, y_resolution, params, rgb_data);
        this->tex_size = {x_resolution, y_resolution};
        this->cubemap = false;
    }
    catch (const RGBEDataLoadingError& e) {
        throw RGBELoadingError(std::string("Failed to read RGBE data: \"") + e.what() + '\"', params.file_path);
    }
}

RGBETexture::RGBETexture(std::string_view file_path) :
        RGBETexture(Parameters{
            GL_LINEAR,
            GL_LINEAR,
            GL_CLAMP_TO_EDGE,
            GL_CLAMP_TO_EDGE,
            std::string(file_path),
            0
        }) {}

RGBETexture::RGBETexture(RGBETexture&& other) noexcept {
    texture_id = other.texture_id;
    tex_size = other.tex_size;

    other.texture_id = 0;
}

RGBETexture& RGBETexture::operator=(RGBETexture &&other) noexcept {
    // This function ignores texture ID of 0.
    glDeleteTextures(1, &texture_id);

    texture_id = other.texture_id;
    tex_size = other.tex_size;

    other.texture_id = 0;

    return *this;
}
