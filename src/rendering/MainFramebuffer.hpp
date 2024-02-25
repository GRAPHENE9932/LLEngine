#pragma once

#include "ManagedRenderbufferID.hpp"
#include "QualitySettings.hpp"
#include "rendering/ExposureController.hpp"
#include "rendering/ManagedFramebufferID.hpp"
#include "rendering/Texture.hpp"

#include <glm/vec2.hpp>

#include <memory>
#include <vector>

namespace llengine {
class BloomRenderer;

class MainFramebuffer {
public:
    MainFramebuffer(glm::u32vec2 size);
    ~MainFramebuffer();

    void render_to_window(float delta_time);

    [[nodiscard]] FramebufferID get_framebuffer_id() const;
    [[nodiscard]] const Texture& get_color_texture() const;

    void apply_postprocessing_settings(const QualitySettings& quality_settings);

    [[nodiscard]] float get_exposure() const;

    void assign_framebuffer_size(glm::u32vec2 size);

private:
    glm::u32vec2 framebuffer_size;

    std::vector<ManagedFramebufferID> framebuffer_lods;
    std::vector<Texture> color_attachment_lods;
    ManagedRenderbufferID depth_attachment;

    bool bloom_enabled = true;
    std::unique_ptr<BloomRenderer> bloom_renderer;

    ExposureController exposure_controller;

    void generate_lods_for_color_attachment();
    void initialize_depth_attachment(glm::u32vec2 size);

    void initialize_framebuffer_lods(glm::u32vec2 size);
};
}