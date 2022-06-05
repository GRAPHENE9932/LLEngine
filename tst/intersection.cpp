#include <gtest/gtest.h>

#include "utils/math.hpp"
#include "macros.hpp"

namespace {
    constexpr float THRESHOLD = 0.00001f;

    TEST(math_test, two_circles_intersection_points) {
        glm::vec2 point_1, point_2;
        utils::IntersectionCount count = utils::two_circles_intersection_points(
            {0.0f, 0.0f}, {2.0f, 2.0f}, 2.0f, point_1, point_2
        );
        EXPECT_EQ(count, utils::IntersectionCount::TWO_POINTS);
        EXPECT_NEAR_2_UNORDERED_V2(point_1, point_2, glm::vec2(0.0f, 2.0f), glm::vec2(2.0f, 0.0f), THRESHOLD);

        count = utils::two_circles_intersection_points(
            {0.0f, 0.0f}, {-2.0f, 3.0f}, 2.0f, point_1, point_2
        );
        EXPECT_EQ(count, utils::IntersectionCount::TWO_POINTS);
        EXPECT_NEAR_2_UNORDERED_V2(
            point_1, point_2,
            glm::vec2(-1.720576692122892f, 1.019615538584738f), glm::vec2(-0.279423307877108f, 1.980384461415261f),
            THRESHOLD
        );

        count = utils::two_circles_intersection_points(
            {1.0f, 1.0f}, {1.0f, 3.0f}, 2.0f, point_1, point_2
        );
        EXPECT_EQ(count, utils::IntersectionCount::TWO_POINTS);
        EXPECT_NEAR_2_UNORDERED_V2(
            point_1, point_2,
            glm::vec2(-0.732050807568877f, 2.0f), glm::vec2(2.732050807568877f, 2.0f),
            THRESHOLD
        );

        count = utils::two_circles_intersection_points(
            {5.0f, 4.0f}, {1.0f, 3.0f}, 2.0f, point_1, point_2
        );
        EXPECT_EQ(count, utils::IntersectionCount::NO_INTERSECTION);

        count = utils::two_circles_intersection_points(
            {1.0f, 3.0f}, {1.0f, 3.0f}, 2.0f, point_1, point_2
        );
        EXPECT_EQ(count, utils::IntersectionCount::INFINITE_POINTS);
    }

    TEST(math_test, two_arcs_intersection_points) {
        glm::vec2 point_1, point_2;
        utils::IntersectionCount count = utils::two_arcs_intersection_points(
            {0.0f, 0.0f}, 1, {2.0f, 3.0f}, 3, 2.0f, point_1, point_2
        );
        EXPECT_EQ(count, utils::IntersectionCount::TWO_POINTS);
        EXPECT_NEAR_2_UNORDERED_V2(
            point_1, point_2,
            glm::vec2(0.279423307877108f, 1.980384461415261f), glm::vec2(1.720576692122892f, 1.019615538584738f),
            THRESHOLD
        );
        
        count = utils::two_arcs_intersection_points(
            {0.0f, 0.0f}, 1, {1.5f, 3.0f}, 3, 2.0f, point_1, point_2
        );
        EXPECT_EQ(count, utils::IntersectionCount::NO_INTERSECTION);
        
        count = utils::two_arcs_intersection_points(
            {-1.0f, 4.0f}, 1, {1.5f, 3.0f}, 2, 2.0f, point_1, point_2
        );
        EXPECT_EQ(count, utils::IntersectionCount::ONE_POINT);
        EXPECT_NEAR_V2(
            point_1, glm::vec2(0.799294218002552f, 4.873235545006379f), THRESHOLD
        );

        count = utils::two_arcs_intersection_points(
            {-1.0f, 4.0f}, 2, {-1.0f, 4.0f}, 2, 2.0f, point_1, point_2
        );
        EXPECT_EQ(count, utils::IntersectionCount::INFINITE_POINTS);
        
        count = utils::two_arcs_intersection_points(
            {-1.0f, 4.0f}, 2, {-1.0f, 4.0f}, 4, 2.0f, point_1, point_2
        );
        EXPECT_EQ(count, utils::IntersectionCount::NO_INTERSECTION);

        count = utils::two_arcs_intersection_points(
            {-1.0f, 4.0f}, 2, {-1.0f, 4.0f}, 3, 2.0f, point_1, point_2
        );
        EXPECT_EQ(count, utils::IntersectionCount::ONE_POINT);
        EXPECT_NEAR_V2(point_1, glm::vec2(-3.0f, 4.0), THRESHOLD);
    }
}
