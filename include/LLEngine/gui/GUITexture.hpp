#pragma once

#include "rendering/Texture.hpp"

#include <glm/vec4.hpp>

#include <memory>

namespace llengine {
struct GUITexture {
    std::shared_ptr<Texture> texture = nullptr;
    float left_border = 0.0f;
    float right_border = 1.0f;
    float top_border = 0.0f;
    float bottom_border = 1.0f;
    glm::vec4 color_factor = {1.0f, 1.0f, 1.0f, 1.0f};

    [[nodiscard]] std::shared_ptr<Texture>& operator->() {
        return texture;
    }
    [[nodiscard]] const std::shared_ptr<Texture>& operator->() const {
        return texture;
    }

    [[nodiscard]] Texture& operator*() {
        return *texture;
    }
    [[nodiscard]] const Texture& operator*() const {
        return *texture;
    }

    [[nodiscard]] bool empty() const noexcept {
        return texture == nullptr;
    }
};
}