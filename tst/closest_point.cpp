#include <gtest/gtest.h>

#include "utils/math.hpp"

namespace {
    TEST(math_test, closest_point_to_hor_line_segment) {
        glm::vec2 result = utils::closest_point_to_hor_line_segment(
            {1.0f, 2.0f}, 0.0f, 2.0f, 1.0f
        );

        EXPECT_EQ(result, glm::vec2(1.0f, 1.0f));
    }
}
