#pragma once

#include "ManagedRenderbufferID.hpp"
#include "QualitySettings.hpp"
#include "rendering/ManagedFramebufferID.hpp"
#include "rendering/Texture.hpp"

#include <glm/vec2.hpp>

namespace llengine {
class BloomRenderer;

class MainFramebuffer {
public:
    MainFramebuffer(glm::u32vec2 size);
    ~MainFramebuffer();

    void render_to_window(float delta_time);

    [[nodiscard]] FramebufferID get_framebuffer_id() const;
    [[nodiscard]] TextureID get_color_texture_id() const;

    void apply_postprocessing_settings(const QualitySettings& quality_settings);

    [[nodiscard]] float get_exposure() const;

    void assign_framebuffer_size(glm::u32vec2 size);

private:
    glm::u32vec2 framebuffer_size;

    ManagedFramebufferID framebuffer;
    ManagedTextureID color_attachment;
    ManagedRenderbufferID depth_attachment;

    bool bloom_enabled = true;
    std::unique_ptr<BloomRenderer> bloom_renderer;

    float exposure = 0.0f;

    void initialize_color_attachment(glm::u32vec2 size);
    void initialize_depth_attachment(glm::u32vec2 size);

    void compute_automatic_exposure(float delta_time);

    void initialize_framebuffer(glm::u32vec2 size);
};
}