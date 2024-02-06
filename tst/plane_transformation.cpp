#include "math/Plane.hpp"
#include "testing_tools.hpp"

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/quaternion.hpp>
#include <gtest/gtest.h>

TEST(PlaneTransformation, IdentityTransform) {
    llengine::Plane plane;
    plane.normal = { 0.0f, 1.0f, 0.0f };
    plane.distance = 0.0f;

    glm::mat4 identity(1.0f);
    llengine::Plane transformed_plane = plane.transformed(identity);

    expect_near_vec3(transformed_plane.normal, { 0.0f, 1.0f, 0.0f }, 0.001f);
    EXPECT_NEAR(transformed_plane.distance, 0.0f, 0.001f);
}

TEST(PlaneTransformation, CombinedTranslationAndRotation) {
    llengine::Plane plane { { 0.7071067812f, 0.7071067812f, 0.0f }, 2.0f};

    glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3(1.5f, 2.0f, -3.5f));
    glm::mat4 rotation = glm::toMat4(glm::quat(glm::vec3(glm::radians(30.0f), glm::radians(45.0f), glm::radians(-60.0f))));
    glm::mat4 combined = translation * rotation;

    llengine::Plane transformed_plane = plane.transformed(combined);

    expect_near_vec3(transformed_plane.normal, { 0.905330569f, -0.3433303238, -0.2500016992f }, 0.001f);
    EXPECT_NEAR(transformed_plane.distance, 3.546324603f, 0.001f);
}