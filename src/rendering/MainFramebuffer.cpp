#include "MainFramebuffer.hpp"
#include "BloomRenderer.hpp"
#include "datatypes.hpp"
#include "rendering/ExposureController.hpp"
#include "rendering/Shader.hpp"
#include "rendering/Mesh.hpp"
#include "rendering/Texture.hpp"

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

#include <cmath>

namespace llengine {
static std::size_t calculate_amount_of_lods(glm::u32vec2 size) {
    return static_cast<std::size_t>(
        std::floor(
            std::log2(std::max(size.x, size.y))
        ) + 1
    );
}

static std::uint32_t calculate_lod_for_exposure_controller(glm::u32vec2 main_fb_size) {
    return std::min(4ul, calculate_amount_of_lods(main_fb_size) - 1ul);
}

static glm::u32vec2 calculate_size_for_exposure_controller(glm::u32vec2 main_fb_size) {
    return main_fb_size / static_cast<std::uint32_t>(std::pow(2, calculate_lod_for_exposure_controller(main_fb_size)));
}

MainFramebuffer::MainFramebuffer(glm::u32vec2 size) : bloom_renderer(nullptr),
    framebuffer_size(size),
    exposure_controller(calculate_size_for_exposure_controller(size)) {
    initialize_framebuffer_lods(size);
}

MainFramebuffer::~MainFramebuffer() {

}

void MainFramebuffer::render_to_window(float delta_time) {
    generate_lods_for_color_attachment();
    exposure_controller.recompute_exposure(
        color_attachment_lods.at(calculate_lod_for_exposure_controller(framebuffer_size)),
        delta_time
    );

    if (bloom_enabled) {
        if (!bloom_renderer) {
            bloom_renderer = std::make_unique<BloomRenderer>(framebuffer_size);
        }
        bloom_renderer->render_to_bloom_texture(color_attachment_lods, 0.00375f);
    }

    glDisable(GL_DEPTH_TEST);
    static Shader<"main_image", "bloom_image", "exposure"> postprocessing_shader(
        #include "shaders/postprocessing/postprocessing.vert"
        ,
        #include "shaders/postprocessing/postprocessing.frag"
    );
    postprocessing_shader.use_shader();
    postprocessing_shader.bind_2d_texture<"main_image">(get_color_texture().get_id(), 0);
    postprocessing_shader.set_float<"exposure">(get_exposure());
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
    return framebuffer_lods.at(0).get();
}

[[nodiscard]] const Texture& MainFramebuffer::get_color_texture() const {
    return color_attachment_lods.at(0);
}

void MainFramebuffer::apply_postprocessing_settings(const QualitySettings& quality_settings) {
    bloom_enabled = quality_settings.enable_bloom;
}

[[nodiscard]] float MainFramebuffer::get_exposure() const {
    return exposure_controller.get_exposure();
}

void MainFramebuffer::assign_framebuffer_size(glm::u32vec2 size) {
    if (bloom_enabled && bloom_renderer) {
        bloom_renderer->assign_framebuffer_size(size);
    }
    if (this->framebuffer_size == size) {
        return;
    }

    this->framebuffer_size = size;
    initialize_framebuffer_lods(size);
    exposure_controller = ExposureController(calculate_size_for_exposure_controller(size));
}

static Texture initialize_color_attachment_lod(glm::u32vec2 size) {
    ManagedTextureID tex_id;
    glGenTextures(1, &tex_id.get_ref());
    glBindTexture(GL_TEXTURE_2D, tex_id);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_R11F_G11F_B10F, size.x, size.y, 0,
        GL_RGB, GL_FLOAT, nullptr
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    return Texture::from_texture_id(std::move(tex_id), size, Texture::Type::TEX_2D);
}

void MainFramebuffer::initialize_depth_attachment(glm::u32vec2 size) {
    depth_attachment.delete_renderbuffer();
    glGenRenderbuffers(1, &depth_attachment.get_ref());
    glBindRenderbuffer(GL_RENDERBUFFER, depth_attachment);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, size.x, size.y);
}

static glm::vec3 compute_average_color(const std::vector<Texture>& color_attachment_lods) {
    glBindTexture(GL_TEXTURE_2D, color_attachment_lods.back().get_id());

    glm::vec4 result;
    glGetTexImage(
        GL_TEXTURE_2D,
        0,
        GL_RGB,
        GL_FLOAT,
        glm::value_ptr(result)
    );

    return glm::vec3(result);
}

void MainFramebuffer::generate_lods_for_color_attachment() {
    glDisable(GL_DEPTH_TEST);
    static Shader<"previous_lod"> main_fb_color_downsample_shader(
        #include "shaders/postprocessing/main_fb_color_downsample.vert"
        ,
        #include "shaders/postprocessing/main_fb_color_downsample.frag"
    );

    main_fb_color_downsample_shader.use_shader();
    for (std::size_t lod = 1; lod < color_attachment_lods.size(); lod++) {
        main_fb_color_downsample_shader.bind_2d_texture<"previous_lod">(color_attachment_lods[lod - 1].get_id(), 0);

        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_lods.at(lod));

        glViewport(0, 0, color_attachment_lods[lod].get_size().x, color_attachment_lods[lod].get_size().y);
        Mesh::get_quad()->bind_vao(true, false, false);
        glDrawArrays(GL_TRIANGLES, 0, Mesh::get_quad()->get_amount_of_vertices());
        Mesh::get_quad()->unbind_vao(true, false, false);
    }
    glEnable(GL_DEPTH_TEST);
}

void MainFramebuffer::initialize_framebuffer_lods(glm::u32vec2 size) {
    framebuffer_lods.clear();
    color_attachment_lods.clear();

    initialize_depth_attachment(size);

    std::size_t lods_amount = calculate_amount_of_lods(size);
    framebuffer_lods.reserve(lods_amount);
    color_attachment_lods.reserve(lods_amount);

    glm::u32vec2 cur_size = size;
    for (std::size_t lod = 0; lod < lods_amount; lod++) {
        framebuffer_lods.emplace_back();
        glGenFramebuffers(1, &framebuffer_lods[lod].get_ref());
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_lods[lod]);

        color_attachment_lods.emplace_back(std::move(initialize_color_attachment_lod(cur_size)));
        GraphicsAPIEnum attachment = GL_COLOR_ATTACHMENT0;
        glDrawBuffers(1, &attachment);

        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, color_attachment_lods[lod].get_id(), 0);
        if (lod == 0) {
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_attachment);
        }

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            throw std::runtime_error("Failed to initialize the main framebuffer.");
        }

        cur_size /= 2;
        cur_size = glm::max(cur_size, glm::u32vec2(1, 1));
    }
}
}