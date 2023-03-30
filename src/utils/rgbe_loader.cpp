#include "texture_tools.hpp"

#include <fmt/format.h>

#include <vector>
#include <cmath>
#include <fstream>
#include <array>
#include <optional>
#include <sstream>
#include <queue>
#include <mutex>
#include <thread>
#include <span>
#include <algorithm>
#include <map>

constexpr std::string_view RGBE_IDENTIFIER {"#?RADIANCE"};
constexpr std::streamsize MAX_CHUNK_SIZE {16ll * 1024ll * 1024ll};
constexpr std::array<std::uint8_t, 2> NEW_SCANLINE_MARK {0x02, 0x02};

std::streamsize calculate_bytes_left(std::ifstream& stream) {
    const auto current_position {stream.tellg()};
    stream.seekg(0, stream.end);
    const auto result = stream.tellg() - current_position;
    stream.seekg(current_position);
    return result;
}

template<typename Iter1, typename Iter2>
std::size_t count_occurences(Iter1 whole_begin, Iter1 whole_end, Iter2 part_begin, Iter2 part_end) {
    std::size_t count {0};
    Iter1 current_begin = whole_begin;

    auto search_result = std::search(current_begin, whole_end, part_begin, part_end);
    while (search_result != whole_end) {
        current_begin = search_result + (part_end - part_begin);
        count++;
        search_result = std::search(current_begin, whole_end, part_begin, part_end);
    }

    return count;
}

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

// Returns size of the encoded scanline in bytes.
std::uint32_t load_scanline(const std::span<std::uint8_t>& chunk, const std::span<float>& output, std::uint32_t image_width) {
    std::uint32_t offset_in_scanline {0};

    std::uint16_t scanline_width = chunk[offset_in_scanline + 2] << 8 | chunk[offset_in_scanline + 3];
    offset_in_scanline += 4;
    if (scanline_width != image_width) {
        throw TextureLoadingError("Failed to load RGBE data: ambiguous texture width.");
    }

    std::vector<std::uint8_t> scanline_buffer(image_width * 4);
    for (std::uint32_t cur_column = 0; cur_column < image_width * 4;) {
        std::uint8_t run_length {chunk[offset_in_scanline]};
        offset_in_scanline++;

        if (run_length > 128) {
            // A run of the same value.
            run_length -= 128;

            if (run_length == 0 || scanline_buffer.size() < cur_column + run_length) {
                throw TextureLoadingError("Failed to load RGBE data: invalid run length in run-length encoded data.");
            }

            std::uint8_t value {chunk[offset_in_scanline]};
            offset_in_scanline++;
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
                throw TextureLoadingError("Failed to load RGBE data: invalid run length in run-length encoded data.");
            }

            std::copy(
                chunk.begin() + offset_in_scanline,
                chunk.begin() + offset_in_scanline + run_length,
                scanline_buffer.begin() + cur_column
            );

            offset_in_scanline += run_length;
            cur_column += run_length;
        }
    }

    rgbe_to_rgb(
        scanline_buffer.begin(), scanline_buffer.end(), output.begin()
    );

    return offset_in_scanline;
}

struct InputChunk {
    InputChunk(
        const std::span<std::uint8_t>& data_span,
        std::unique_ptr<std::uint8_t[]>&& data,
        std::uint32_t chunk_index
    ) : data_span(data_span), data(std::move(data)), chunk_index(chunk_index) {}
    InputChunk(InputChunk&& other) = default;
    InputChunk& operator=(InputChunk&& other) = default;

    std::span<std::uint8_t> data_span;
    std::unique_ptr<std::uint8_t[]> data;
    std::uint32_t chunk_index;
};

struct OutputChunk {
    OutputChunk(
        const std::span<float>& data_span,
        std::unique_ptr<float[]>&& data,
        std::uint32_t scanlines_count
    ) : data_span(data_span), data(std::move(data)), scanlines_count(scanlines_count) {}
    OutputChunk(OutputChunk&& other) = default;
    OutputChunk& operator=(OutputChunk&& other) = default;

    std::span<float> data_span;
    std::unique_ptr<float[]> data;
    std::uint32_t scanlines_count;
};

struct ThreadSharedVariables {
    std::uint32_t image_width = 0; // Constant during the job. Can be read without locking.
    std::mutex mutex;
    std::queue<InputChunk> input_chunks_queue; // Must be accessed with mutex.
    std::map<std::uint32_t, OutputChunk> output_chunks_map; // Must be accessed with mutex.
    bool chunks_over = false; // Must be accessed with mutex.
};

template<typename T>
struct OwningSpan {
    OwningSpan(std::size_t size) {
        ptr = std::make_unique_for_overwrite<T[]>(size);
        span = std::span(ptr.get(), size);
    }

    std::span<T> span;
    std::unique_ptr<T[]> ptr;
};

void read_run_length_encoded_data_single_thread(ThreadSharedVariables& vars) {
    using namespace std::chrono_literals;

    while (true) {
        vars.mutex.lock();
        if (vars.input_chunks_queue.empty()) {
            if (vars.chunks_over) {
                vars.mutex.unlock();
                break;
            }

            vars.mutex.unlock();
            std::this_thread::sleep_for(2ms);
            continue;
        }

        // Take one chunk.
        auto chunk_in(std::move(vars.input_chunks_queue.front()));
        vars.input_chunks_queue.pop();
        vars.mutex.unlock();

        // Allocate output chunk.
        auto scanlines_in_chunk {count_occurences(
            chunk_in.data_span.begin(), chunk_in.data_span.end(), NEW_SCANLINE_MARK.begin(), NEW_SCANLINE_MARK.end()
        )};
        std::unique_ptr<float[]> output_chunk_data {std::make_unique_for_overwrite<float[]>(scanlines_in_chunk * vars.image_width * 3)};
        std::span<float> output_chunk_data_span(output_chunk_data.get(), scanlines_in_chunk * vars.image_width * 3);

        std::size_t offset_in_chunk {0};
        for (std::uint32_t scanline_i = 0; scanline_i < scanlines_in_chunk; scanline_i++) {
            offset_in_chunk += load_scanline(
                chunk_in.data_span.subspan(offset_in_chunk),
                output_chunk_data_span.subspan(scanline_i * vars.image_width * 3),
                vars.image_width
            );
        }

        // Add output chunk to the list.
        vars.mutex.lock();
        vars.output_chunks_map.emplace(std::make_pair(chunk_in.chunk_index, OutputChunk(output_chunk_data_span, std::move(output_chunk_data), scanlines_in_chunk)));
        vars.mutex.unlock();
    }
}

OwningSpan<float> read_run_length_encoded_data(
    std::uint32_t width,
    std::uint32_t height,
    std::ifstream& stream
) {
    const unsigned int threads_count {std::thread::hardware_concurrency() == 0 ? 4 : std::thread::hardware_concurrency()};

    ThreadSharedVariables vars;
    vars.image_width = width;

    std::vector<std::thread> threads;
    threads.reserve(threads_count);
    for (unsigned int i = 0; i < threads_count; i++) {
        threads.emplace_back(read_run_length_encoded_data_single_thread, std::ref(vars));
    }

    std::streamsize bytes_left {calculate_bytes_left(stream)};
    const std::streamsize max_chunk_size = std::clamp(
        (height / threads_count) * width,
        width * 4 * 2 + 1, // The maximum possible run-length encoded scanline size in bytes + 1.
        static_cast<std::uint32_t>(MAX_CHUNK_SIZE)
    );

    std::uint32_t index_of_chunk {0};
    while (bytes_left > 0) {
        bool is_last_chunk {max_chunk_size > bytes_left};
        std::uint32_t cur_chunk_size = std::min(max_chunk_size, bytes_left);
    
        // I would use std::vector here, but std::vector initializes all the values and this is very slow.
        std::unique_ptr<std::uint8_t[]> chunk_ptr {std::make_unique_for_overwrite<std::uint8_t[]>(cur_chunk_size)};
        std::span<std::uint8_t> chunk(chunk_ptr.get(), cur_chunk_size);
        stream.read(reinterpret_cast<char*>(chunk.data()), cur_chunk_size);
        bytes_left -= cur_chunk_size;
        if (!stream) {
            throw TextureLoadingError("Failed to read a chunk of data in the RGBE file stream.");
        }

        // Chunk must be aligned with scanlines, so find the start of the last scanline that is
        // likely corrupted (because we load just a chunk, not a whole file).
        if (!is_last_chunk) {
            const auto search_result = std::search(
                chunk.rbegin(), chunk.rend(),
                NEW_SCANLINE_MARK.begin(), NEW_SCANLINE_MARK.end()
            );
            if (search_result == chunk.rend()) {
                throw TextureLoadingError("Can't find new scanline mark in the RGBE chunk.");
            }
            const auto end_of_the_first_scanline = search_result + (NEW_SCANLINE_MARK.size()/* + 1*/);
            const auto alignment_difference {std::distance(chunk.rbegin(), end_of_the_first_scanline)};
            cur_chunk_size -= alignment_difference;

            chunk = chunk.subspan(0, cur_chunk_size);
            stream.seekg(-alignment_difference, stream.cur);
            bytes_left += alignment_difference;
        }

        // Add chunk to the queue.
        vars.mutex.lock();
        vars.input_chunks_queue.emplace(chunk, std::move(chunk_ptr), index_of_chunk);
        vars.mutex.unlock();

        index_of_chunk++;
    }
    vars.mutex.lock();
    vars.chunks_over = true;
    vars.mutex.unlock();

    for (auto& thread : threads) {
        thread.join();
    }

    // Now we have a set with chunks of output data. Firstly, allocate the total output vector...
    OwningSpan<float> total_output(height * width * 3);
    // Secondly, copy everything from set.
    std::uint32_t current_scanline {0};
    for (auto& chunk : vars.output_chunks_map) {
        std::copy(chunk.second.data_span.begin(), chunk.second.data_span.end(), total_output.span.begin() + current_scanline * width * 3);
        chunk.second.data.reset(),
        current_scanline += chunk.second.scanlines_count;
    }
    // And, finally, return the result.
    return total_output;
}

OwningSpan<float> get_rgb_data(
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
        OwningSpan<std::uint8_t> rgbe_data(width * height * 4);
        stream.read(reinterpret_cast<char*>(rgbe_data.span.data()), rgbe_data.span.size());

        if (!stream) {
            throw TextureLoadingError("Failed to read flat data in the RGBE texture.");
        }

        OwningSpan<float> result(width * height * 3);
        rgbe_to_rgb(rgbe_data.span.begin(), rgbe_data.span.end(), result.span.begin());
        return result;
    }
}

GLuint initialize_opengl_texture(
    std::uint32_t width,
    std::uint32_t height,
    const TexLoadingParams& params,
    const OwningSpan<float>& rgb_data
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
        GL_RGB, GL_FLOAT, rgb_data.span.data()
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

std::unique_ptr<Texture> texture_from_rgbe(const TexLoadingParams& params) {
    std::ifstream stream(params.file_path, std::ios::binary | std::ios::in);
    if (!stream) {
        throw TextureLoadingError(fmt::format(
            "Failed to open the RGBE file \"{}\".",
            params.file_path
        ));
    }
    stream.ignore(params.offset);

    // Check identifier.
    std::string identifier;
    std::getline(stream, identifier);
    if (identifier != RGBE_IDENTIFIER) {
        throw TextureLoadingError("Invalid RGBE identifier (magic bytes).");
    }

    // Handle variables.
    bool has_valid_format = false;

    std::string current_line;
    std::getline(stream, current_line);
    while (!current_line.empty()) {
        const auto variable = parse_header_variable(current_line);
        if (!variable.has_value()) {
            throw TextureLoadingError("Failed to parse a line with RGBE variable.");
        }

        if (variable->first == "FORMAT") {
            has_valid_format = variable->second == "32-bit_rle_rgbe";
        }
        // COLORCORR, EXPOSURE, PIXASPECT, VIEW, PRIMARIES are ignored.

        std::getline(stream, current_line);
    }

    if (!has_valid_format) {
        throw TextureLoadingError("Invalid FORMAT in the RGBE texture. Only 32-bit_rle_rgbe is supported.");
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
        throw TextureLoadingError("Invalid resolution string in the RGBE texture.");
    }

    if (x_resolution < 1 || y_resolution < 1) {
        throw TextureLoadingError("Invalid resolution in the RGBE texture.");
    }

    if (x_orientation != "+X" || y_orientation != "-Y") {
        throw TextureLoadingError("Invalid resolution string in the RGBE texture. Only +X -Y resolution is supported.");
    }

    GLuint texture_id;
    glm::u32vec2 tex_size;
    bool is_cubemap;
    OwningSpan<float> rgb_data {get_rgb_data(x_resolution, y_resolution, stream)};
    texture_id = initialize_opengl_texture(x_resolution, y_resolution, params, rgb_data);
    tex_size = {x_resolution, y_resolution};
    is_cubemap = false;

    return std::make_unique<Texture>(texture_id, tex_size, is_cubemap);
}