#include "ExposureController.hpp"
#include "rendering/ManagedFramebufferID.hpp"
#include "rendering/Mesh.hpp"
#include "rendering/Shader.hpp"
#include "rendering/Texture.hpp"

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

namespace llengine {
static std::uint32_t calculate_last_mipmap(glm::u32vec2 size) {
    return static_cast<std::uint32_t>(
        std::floor(std::log2(std::max(size.x, size.y)))
    );
}

static Texture initialize_color_attachment(glm::u32vec2 size) {
    ManagedTextureID texture_id;
    glGenTextures(1, &texture_id.get_ref());
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_R16, size.x,
        size.y, 0, GL_RED, GL_FLOAT, nullptr
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, calculate_last_mipmap(size));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    return Texture::from_texture_id(std::move(texture_id), size, Texture::Type::TEX_2D);
}

static ManagedFramebufferID initialize_framebuffer(const Texture& color_attachment) {
    ManagedFramebufferID fb_id;
    glGenFramebuffers(1, &fb_id.get_ref());
    glBindFramebuffer(GL_FRAMEBUFFER, fb_id);

    GraphicsAPIEnum attachment = GL_COLOR_ATTACHMENT0;
    glDrawBuffers(1, &attachment);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_attachment, 0);
    
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        throw std::runtime_error("Failed to initialize the luminance framebuffer.");
    }

    return fb_id;
}

ExposureController::ExposureController(glm::u32vec2 luminance_fb_size) {
    luminance_attachment = initialize_color_attachment(luminance_fb_size);
    luminance_framebuffer = initialize_framebuffer(luminance_attachment);
}

void ExposureController::render_to_luminance_fb(const Texture& main_color_texture) const {
    static Shader<"source_texture"> shader(
        #include "shaders/luminance_computation_shader.vert"
        ,
        #include "shaders/luminance_computation_shader.frag"
    );

    shader.use_shader();
    shader.bind_2d_texture<"source_texture">(main_color_texture, 0);

    glViewport(0, 0, luminance_attachment.get_size().x, luminance_attachment.get_size().y);
    glBindFramebuffer(GL_FRAMEBUFFER, luminance_framebuffer);

    Mesh::get_quad()->bind_vao(true, false, false);
    glDrawArrays(GL_TRIANGLES, 0, Mesh::get_quad()->get_amount_of_vertices());
    Mesh::get_quad()->unbind_vao(true, false, false);
}

float ExposureController::extract_average_luminance() const {
    glBindTexture(GL_TEXTURE_2D, luminance_attachment);
    glGenerateMipmap(GL_TEXTURE_2D);

    glm::vec4 result;
    glGetTexImage(
        GL_TEXTURE_2D,
        calculate_last_mipmap(luminance_attachment.get_size()),
        GL_RED,
        GL_FLOAT,
        glm::value_ptr(result)
    );

    return result.r;
}

constexpr float EXPOSURE_KEY_VALUE = 0.18f;
constexpr float DECAY_RATE = 0.75f;

void ExposureController::recompute_exposure(const Texture& main_color_texture, float delta_time) {
    render_to_luminance_fb(main_color_texture);
    float luminance = extract_average_luminance();

    float target_exposure = EXPOSURE_KEY_VALUE / luminance;

    exposure = exposure + (target_exposure - exposure) * (1.0f - std::exp(-delta_time * DECAY_RATE));
}
}