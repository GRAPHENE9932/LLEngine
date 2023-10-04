#include "MainFramebuffer.hpp"
#include "BloomRenderer.hpp"
#include "rendering/Shader.hpp"
#include "rendering/Mesh.hpp"

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

#include <cmath>

namespace llengine {
MainFramebuffer::MainFramebuffer(glm::u32vec2 size) : bloom_renderer(nullptr), framebuffer_size(size) {
    initialize_framebuffer(size);
}

MainFramebuffer::~MainFramebuffer() {

}

void MainFramebuffer::render_to_window(float delta_time) {
    compute_automatic_exposure(delta_time);

    if (bloom_enabled) {
        if (!bloom_renderer) {
            bloom_renderer = std::make_unique<BloomRenderer>(framebuffer_size);
        }
        bloom_renderer->render_to_bloom_texture(get_color_texture_id(), 0.005f);
    }

    glDisable(GL_DEPTH_TEST);
    static Shader<"main_image", "bloom_image", "exposure"> postprocessing_shader(
        #include "shaders/postprocessing.vert"
        ,
        #include "shaders/postprocessing.frag"
    );
    postprocessing_shader.use_shader();
    postprocessing_shader.bind_2d_texture<"main_image">(get_color_texture_id(), 0);
    postprocessing_shader.set_float<"exposure">(exposure);
    if (bloom_enabled) {
        postprocessing_shader.bind_2d_texture<"bloom_image">(bloom_renderer->get_bloom_texture_id(), 1);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    Mesh::get_quad()->bind_vao(true, false, false);
    glDrawArrays(GL_TRIANGLES, 0, Mesh::get_quad()->get_amount_of_vertices());
    Mesh::get_quad()->unbind_vao(true, false, false);
    glEnable(GL_DEPTH_TEST);
}

[[nodiscard]] FramebufferID MainFramebuffer::get_framebuffer_id() const {
    return framebuffer.get();
}

[[nodiscard]] TextureID MainFramebuffer::get_color_texture_id() const {
    return color_attachment.get();
}

void MainFramebuffer::apply_postprocessing_settings(const QualitySettings& quality_settings) {
    bloom_enabled = quality_settings.enable_bloom;
}

[[nodiscard]] float MainFramebuffer::get_exposure() const {
    return exposure;
}

void MainFramebuffer::assign_framebuffer_size(glm::u32vec2 size) {
    if (bloom_enabled && bloom_renderer) {
        bloom_renderer->assign_framebuffer_size(size);
    }
    if (this->framebuffer_size == size) {
        return;
    }

    this->framebuffer_size = size;
    initialize_framebuffer(size);
}

void MainFramebuffer::initialize_color_attachment(glm::u32vec2 size) {
    color_attachment.delete_texture();
    glGenTextures(1, &color_attachment.get_ref());
    glBindTexture(GL_TEXTURE_2D, color_attachment);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_R11F_G11F_B10F, size.x, size.y, 0,
        GL_RGB, GL_FLOAT, nullptr
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, color_attachment, 0);
}

void MainFramebuffer::initialize_depth_attachment(glm::u32vec2 size) {
    depth_attachment.delete_renderbuffer();
    glGenRenderbuffers(1, &depth_attachment.get_ref());
    glBindRenderbuffer(GL_RENDERBUFFER, depth_attachment);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, size.x, size.y);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_attachment);
}

static std::uint32_t calculate_amount_of_mipmap_levels(glm::u32vec2 texture_size) {
    return static_cast<std::uint32_t>(
        std::floor(
            std::log2(static_cast<float>(std::max(texture_size.x, texture_size.y)))
        ) + 1
    );
}

static glm::vec3 compute_average_texture_color(TextureID texture_id, glm::u32vec2 texture_size) {
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glGenerateMipmap(GL_TEXTURE_2D);

    glm::vec3 result;
    glGetTexImage(
        GL_TEXTURE_2D,
        calculate_amount_of_mipmap_levels(texture_size) - 1,
        GL_RGB,
        GL_FLOAT,
        glm::value_ptr(result)
    );

    return result;
}

constexpr float EXPOSURE_KEY_VALUE = 0.18f;
constexpr float DECAY_RATE = 0.75f;

void MainFramebuffer::compute_automatic_exposure(float delta_time) {
    const glm::vec3 average_color = compute_average_texture_color(get_color_texture_id(), framebuffer_size);
    float average_luminance = 0.2126f * average_color.r + 0.7152f * average_color.g + 0.0722f * average_color.b;

    float target_exposure = EXPOSURE_KEY_VALUE / average_luminance;

    exposure = exposure + (target_exposure - exposure) * (1.0f - std::exp(-delta_time * DECAY_RATE));
}

void MainFramebuffer::initialize_framebuffer(glm::u32vec2 size) {
    framebuffer.delete_framebuffer();
    glGenFramebuffers(1, &framebuffer.get_ref());
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    initialize_color_attachment(size);
    initialize_depth_attachment(size);

    GraphicsAPIEnum attachment = GL_COLOR_ATTACHMENT0;
    glDrawBuffers(1, &attachment);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        throw std::runtime_error("Failed to initialize main framebuffer.");
    }
}
}