#include "nodes/rendering/CameraNode.hpp"
#include "testing_tools.hpp"

#include <fmt/format.h>
#include <gtest/gtest.h>

TEST(FrustumConstruction, IdentityTransform) {
    llengine::CameraNode camera;
    camera.set_translation({ 0.0f, 0.0f, 0.0f});
    camera.set_rotation(glm::quat({ 0.0f, 0.0f, 0.0f }));
    camera.set_field_of_view(glm::radians(70.0f));
    camera.set_near_distance(0.2f);
    camera.set_far_distance(15.0f);
    camera.set_aspect_ratio(2.711272110f);

    llengine::Frustum frustum = camera.get_frustum();
    
    // Near plane
    expect_near_vec3(frustum.planes.near.normal, { 0.0f, 0.0f, 1.0f }, 0.001f);
    EXPECT_NEAR(frustum.planes.near.distance, 0.2f, 0.001f);

    // Far plane
    expect_near_vec3(frustum.planes.far.normal, { 0.0f, 0.0f, -1.0f }, 0.001f);
    EXPECT_NEAR(frustum.planes.far.distance, -15.0f, 0.001f);

    // Left plane
    expect_near_vec3(frustum.planes.left.normal, { -0.466045f, 0.0f, 0.884761f }, 0.001f);
    EXPECT_NEAR(frustum.planes.left.distance, 0.0f, 0.001f);

    // Right plane
    expect_near_vec3(frustum.planes.right.normal, { 0.466045f, 0.0f, 0.884761f }, 0.001f);
    EXPECT_NEAR(frustum.planes.right.distance, 0.0f, 0.001f);

    // Top plane
    expect_near_vec3(frustum.planes.top.normal, { 0.0f, -0.819152f, 0.573576 }, 0.001f);
    EXPECT_NEAR(frustum.planes.top.distance, 0.0f, 0.001f);

    // Bottom plane
    expect_near_vec3(frustum.planes.bottom.normal, { 0.0f, 0.819152f, 0.573576f }, 0.001f);
    EXPECT_NEAR(frustum.planes.bottom.distance, 0.0f, 0.001f);
}

TEST(FrustumConstruction, WithTranslationAndRotation) {
    llengine::CameraNode camera;
    camera.set_translation({ 1.5f, 2.0f, -3.5f});
    camera.set_rotation(glm::quat({ glm::radians(-30.0f), glm::radians(45.0f), glm::radians(60.0f) }));
    camera.set_field_of_view(glm::radians(70.0f));
    camera.set_near_distance(0.2f);
    camera.set_far_distance(15.0f);
    camera.set_aspect_ratio(2.711272110f);

    llengine::Frustum frustum = camera.get_frustum();
    
    // Near plane
    llengine::Plane IDENTITY_NEAR_PLANE = { { 0.0f, 0.0f, 1.0f }, 0.2f };
    expect_near_vec3(frustum.planes.near.normal, IDENTITY_NEAR_PLANE.transformed(camera.get_global_matrix()).normal, 0.001f);
    EXPECT_NEAR(frustum.planes.near.distance, IDENTITY_NEAR_PLANE.transformed(camera.get_global_matrix()).distance, 0.001f);

    // Far plane
    llengine::Plane IDENTITY_FAR_PLANE = { { 0.0f, 0.0f, -1.0f }, -15.0f };
    expect_near_vec3(frustum.planes.far.normal, IDENTITY_FAR_PLANE.transformed(camera.get_global_matrix()).normal, 0.001f);
    EXPECT_NEAR(frustum.planes.far.distance, IDENTITY_FAR_PLANE.transformed(camera.get_global_matrix()).distance, 0.001f);

    // Left plane
    llengine::Plane IDENTITY_LEFT_PLANE = { { -0.466045f, 0.0f, 0.884761f }, 0.0f };
    expect_near_vec3(frustum.planes.left.normal, IDENTITY_LEFT_PLANE.transformed(camera.get_global_matrix()).normal, 0.001f);
    EXPECT_NEAR(frustum.planes.left.distance, IDENTITY_LEFT_PLANE.transformed(camera.get_global_matrix()).distance, 0.001f);

    // Right plane
    llengine::Plane IDENTITY_RIGHT_PLANE = { { 0.466045f, 0.0f, 0.884761f }, 0.0f };
    expect_near_vec3(frustum.planes.right.normal, IDENTITY_RIGHT_PLANE.transformed(camera.get_global_matrix()).normal, 0.001f);
    EXPECT_NEAR(frustum.planes.right.distance, IDENTITY_RIGHT_PLANE.transformed(camera.get_global_matrix()).distance, 0.001f);

    // Top plane
    llengine::Plane IDENTITY_TOP_PLANE = { { 0.0f, -0.819152f, 0.573576 }, 0.0f };
    expect_near_vec3(frustum.planes.top.normal, IDENTITY_TOP_PLANE.transformed(camera.get_global_matrix()).normal, 0.001f);
    EXPECT_NEAR(frustum.planes.top.distance, IDENTITY_TOP_PLANE.transformed(camera.get_global_matrix()).distance, 0.001f);

    // Bottom plane
    llengine::Plane IDENTITY_BOTTOM_PLANE = { { 0.0f, 0.819152f, 0.573576f }, 0.0f };
    expect_near_vec3(frustum.planes.bottom.normal, IDENTITY_BOTTOM_PLANE.transformed(camera.get_global_matrix()).normal, 0.001f);
    EXPECT_NEAR(frustum.planes.bottom.distance, IDENTITY_BOTTOM_PLANE.transformed(camera.get_global_matrix()).distance, 0.001f);
}