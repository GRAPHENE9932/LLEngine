#include "rendering/ShadowMap.hpp"

#include <GL/glew.h>

#include <array>

namespace llengine {
constexpr std::array<float, 4> SHADOW_MAP_BORDER_COLOR {1.0f, 1.0f, 1.0f, 1.0f};

ShadowMap::ShadowMap(glm::u32vec2 size) {
    this->size = size;

    glGenFramebuffers(1, &framebuffer.get_ref());
    glGenTextures(1, &texture_id.get_ref());
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, size.x, size.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, SHADOW_MAP_BORDER_COLOR.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture_id, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
}

[[nodiscard]] float ShadowMap::get_adjusted_bias_at_45_deg() const {
    float size_ratio = 1024.0f / size.x;
    float result = bias_at_45_deg * size_ratio;
    return result;
}

void ShadowMap::set_drawing_distance(float new_distance) {
    drawing_distance = new_distance;
}

void ShadowMap::link_to_camera(const CameraNode& camera) {
    this->camera = &camera;
}

[[nodiscard]] glm::mat4 ShadowMap::get_view_proj_matrix() const {
    if (!view_proj_matrix_cache.has_value()) {
        view_proj_matrix_cache = compute_view_proj_matrix();
    }

    return *view_proj_matrix_cache;
}

[[nodiscard]] std::array<glm::vec4, 8> get_camera_frustum_corners(const CameraNode& camera, float max_distance) {
    const glm::mat4 proj_inverse = glm::inverse(camera.get_proj_matrix());
    const glm::mat4 view_inverse = glm::inverse(camera.get_view_matrix());
    const float far_corners_coeff = std::min(max_distance / camera.get_far_distance(), 1.0f);

    std::array<glm::vec4, 8> result;

    for (std::uint8_t corner_index = 0; corner_index < 8; corner_index++) {
        glm::vec4 corner {
            corner_index & 0x01 ? 1.0 : -1.0,
            corner_index & 0x02 ? 1.0 : -1.0,
            corner_index & 0x04 ? 1.0 : -1.0,
            1.0f
        };

        corner = proj_inverse * corner;
        if ((corner_index & 0x04)) {
            corner.x *= far_corners_coeff;
            corner.y *= far_corners_coeff;
            corner.z *= far_corners_coeff;
        }
        corner = view_inverse * corner;

        result[corner_index] = corner / corner.w;
    }

    return result;
}

[[nodiscard]] glm::vec4 compute_average_vector(const std::array<glm::vec4, 8>& vectors) {
    glm::vec4 result {0.0f, 0.0f, 0.0f, 0.0f};
    for (const glm::vec4 cur_vec : vectors) {
        result += cur_vec * (1.0f / 8.0f);
    }
    return result;
}

constexpr float SHADOW_CAMERA_CLEARANCE = 25.0f;

[[nodiscard]] glm::mat4 ShadowMap::compute_view_proj_matrix() const {
    if (camera == nullptr) {
        throw std::runtime_error(
            "Can not compute view projection matrix for shadow mapping as camera is not assigned to the ShadowMap."
        );
    }

    const auto camera_frustum_corners = get_camera_frustum_corners(*camera, drawing_distance * 2.0f);
    const glm::vec3 camera_frustum_center = glm::vec3(compute_average_vector(camera_frustum_corners));
    glm::vec3 flat_frustum_center {camera_frustum_center.x, 0.0f, camera_frustum_center.z};

    const glm::mat4 view { glm::lookAt(
        -light_direction * SHADOW_CAMERA_CLEARANCE + flat_frustum_center,
        flat_frustum_center,
        {0.0f, 1.0f, 0.0f}
    ) };

    const glm::mat4 projection { glm::ortho(
        -drawing_distance,
        drawing_distance,
        -drawing_distance,
        drawing_distance,
        camera_frustum_center.y - SHADOW_CAMERA_CLEARANCE * 2.0f,
        camera_frustum_center.y + SHADOW_CAMERA_CLEARANCE * 2.0f
    ) };

    return projection * view;
}

void ShadowMap::prepare_for_drawing() {
    view_proj_matrix_cache = std::nullopt;

    glViewport(0, 0, get_size().x, get_size().y);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glClear(GL_DEPTH_BUFFER_BIT);
    glDisable(GL_CULL_FACE);
}

void ShadowMap::finish_drawing(
    bool enable_face_culling, FramebufferID default_framebuffer, glm::u32vec2 default_framebuffer_size
) {
    if (enable_face_culling) {
        glEnable(GL_CULL_FACE);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, default_framebuffer);
    const auto framebuffer_size = default_framebuffer_size;
    glViewport(0, 0, framebuffer_size.x, framebuffer_size.y);
}
}