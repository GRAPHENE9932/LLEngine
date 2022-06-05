#include <gtest/gtest.h>

#include "structs/Circle.hpp"
#include "structs/QuadrantArc.hpp"
#include "macros.hpp"

namespace {
    constexpr float THRESHOLD = 0.00001f;

    TEST(math_test, two_circles_intersection_points) {
        glm::vec2 point_1, point_2;

        // {{center.x, center.y}, radius}.
        Circle circle_1 {{0.0f, 0.0f}, 2.0f};
        Circle circle_2 {{2.0f, 2.0f}, 2.0f};
        auto count = circle_1.intersection_points(circle_2, point_1, point_2);
        EXPECT_EQ(count, IntersectionCount::TWO_POINTS);
        EXPECT_NEAR_2_UNORDERED_V2(point_1, point_2, glm::vec2(0.0f, 2.0f), glm::vec2(2.0f, 0.0f), THRESHOLD);

        circle_1 = {{0.0f, 0.0f}, 2.0f};
        circle_2 = {{-2.0f, 3.0f}, 2.0f};
        count = circle_1.intersection_points(circle_2, point_1, point_2);
        EXPECT_EQ(count, IntersectionCount::TWO_POINTS);
        EXPECT_NEAR_2_UNORDERED_V2(
            point_1, point_2,
            glm::vec2(-1.720576692122892f, 1.019615538584738f), glm::vec2(-0.279423307877108f, 1.980384461415261f),
            THRESHOLD
        );

        circle_1 = {{1.0f, 1.0f}, 2.0f};
        circle_2 = {{1.0f, 3.0f}, 2.0f};
        count = circle_1.intersection_points(circle_2, point_1, point_2);
        EXPECT_EQ(count, IntersectionCount::TWO_POINTS);
        EXPECT_NEAR_2_UNORDERED_V2(
            point_1, point_2,
            glm::vec2(-0.732050807568877f, 2.0f), glm::vec2(2.732050807568877f, 2.0f),
            THRESHOLD
        );

        circle_1 = {{5.0f, 4.0f}, 2.0f};
        circle_2 = {{1.0f, 3.0f}, 2.0f};
        count = circle_1.intersection_points(circle_2, point_1, point_2);
        EXPECT_EQ(count, IntersectionCount::NO_INTERSECTION);

        circle_1 = {{1.0f, 3.0f}, 2.0f};
        circle_2 = {{1.0f, 3.0f}, 2.0f};
        count = circle_1.intersection_points(circle_2, point_1, point_2);
        EXPECT_EQ(count, IntersectionCount::INFINITE_POINTS);
    }

    TEST(math_test, two_arcs_intersection_points) {
        glm::vec2 point_1, point_2;

        // {{{center.x, center.y}, radius}, quadrant}.
        QuadrantArc arc_1 {{{0.0f, 0.0f}, 2.0f}, 1};
        QuadrantArc arc_2 {{{2.0f, 3.0f}, 2.0f}, 3};
        auto count = arc_1.intersection_points(arc_2, point_1, point_2);
        EXPECT_EQ(count, IntersectionCount::TWO_POINTS);
        EXPECT_NEAR_2_UNORDERED_V2(
            point_1, point_2,
            glm::vec2(0.279423307877108f, 1.980384461415261f), glm::vec2(1.720576692122892f, 1.019615538584738f),
            THRESHOLD
        );
        
        arc_1 = {{{0.0f, 0.0f}, 2.0f}, 1};
        arc_2 = {{{1.5f, 3.0f}, 2.0f}, 3};
        count = arc_1.intersection_points(arc_2, point_1, point_2);
        EXPECT_EQ(count, IntersectionCount::NO_INTERSECTION);
        
        arc_1 = {{{-1.0f, 4.0f}, 2.0f}, 1};
        arc_2 = {{{1.5f, 3.0f}, 2.0f}, 2};
        count = arc_1.intersection_points(arc_2, point_1, point_2);
        EXPECT_EQ(count, IntersectionCount::ONE_POINT);
        EXPECT_NEAR_V2(
            point_1, glm::vec2(0.799294218002552f, 4.873235545006379f), THRESHOLD
        );

        arc_1 = {{{-1.0f, 4.0f}, 2.0f}, 2};
        arc_2 = {{{-1.0f, 4.0f}, 2.0f}, 2};
        count = arc_1.intersection_points(arc_2, point_1, point_2);
        EXPECT_EQ(count, IntersectionCount::INFINITE_POINTS);
        
        arc_1 = {{{-1.0f, 4.0f}, 2.0f}, 2};
        arc_2 = {{{-1.0f, 4.0f}, 2.0f}, 4};
        count = arc_1.intersection_points(arc_2, point_1, point_2);
        EXPECT_EQ(count, IntersectionCount::NO_INTERSECTION);

        arc_1 = {{{-1.0f, 4.0f}, 2.0f}, 2};
        arc_2 = {{{-1.0f, 4.0f}, 2.0f}, 3};
        count = arc_1.intersection_points(arc_2, point_1, point_2);
        EXPECT_EQ(count, IntersectionCount::ONE_POINT);
        EXPECT_NEAR_V2(point_1, glm::vec2(-3.0f, 4.0), THRESHOLD);
    }
}
