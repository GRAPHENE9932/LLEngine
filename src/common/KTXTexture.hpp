#pragma once

#include <string>
#include <vector>
#include <stdint.h>

#include "Texture.hpp"

class KTXTexture : public Texture {
public:
    /// Loads texture from KTX2 file.
    /// Not supported:
    ///   - Multiple layers
    ///   - Multiple faces (cubemaps)
    ///   - Supercompression other than Zstandard.
    KTXTexture(const std::string& file_path);

private:
    std::vector<char> dfd_block;
    std::vector<std::pair<std::string, std::string>> key_value_data;
    std::vector<char> supercompression_global_data;
};
