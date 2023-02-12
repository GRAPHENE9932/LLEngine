#pragma once

#include <string> // std::string
#include <vector> // std::vector
#include <string_view> // std::string_view

#include "Texture.hpp"

class KTXTexture : public Texture {
public:
    /// Loads texture from KTX2 file.
    /// Not supported:
    ///   - Multiple layers
    ///   - Supercompression other than Zstandard.
    explicit KTXTexture(const Parameters& params);
    /// Loads texture from KTX2 file.
    /// Not supported:
    ///   - Multiple layers
    ///   - Supercompression other than Zstandard.
    explicit KTXTexture(std::string_view file_path);
    KTXTexture(const KTXTexture& other) = delete;
    KTXTexture(KTXTexture&& other) noexcept;
    KTXTexture& operator=(KTXTexture&& other) noexcept;

private:
    std::vector<char> dfd_block;
    std::vector<std::pair<std::string, std::string>> key_value_data;
    std::vector<char> supercompression_global_data;
};
