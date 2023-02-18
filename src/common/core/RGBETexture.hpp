#pragma once

#include "Texture.hpp"

class RGBETexture : public Texture {
public:
    explicit RGBETexture(const Parameters& params);
    explicit RGBETexture(std::string_view file_path);
    RGBETexture(const RGBETexture& other) = delete;
    RGBETexture(RGBETexture&& other) noexcept;
    RGBETexture& operator=(RGBETexture&& other) noexcept;
};
