#include <gtest/gtest.h>

#include "structs/Circle.hpp"
#include "structs/HorLine.hpp"
#include "structs/VertLine.hpp"
#include "structs/QuadrantArc.hpp"
#include "utils/math.hpp"
#include "macros.hpp"

namespace {
    constexpr float THRESHOLD = 0.00001f;

    TEST(math_test, two_circles_intersection_points) {
        glm::vec2 point_1, point_2;

        // {{center.x, center.y}, radius}.
        Circle circle_1 {{0.0f, 0.0f}, 2.0f};
        Circle circle_2 {{2.0f, 2.0f}, 2.0f};
        auto count {circle_1.intersection_points(circle_2, point_1, point_2)};
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
        auto count {arc_1.intersection_points(arc_2, point_1, point_2)};
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

    TEST(math_test, arc_and_vert_line_intersection_points) {
        glm::vec2 point_1;

        // {{{center.x, center.y}, radius}, quadrant}.
        QuadrantArc arc {{{2.0f, 1.0f}, 3.0f}, 4};
        // {min_x, max_x, y}.
        VertLine line {-1.0f, 5.0f, 2.0f};
        auto count {arc.intersection_points(line, point_1)};
        EXPECT_EQ(count, IntersectionCount::NO_INTERSECTION);
        
        arc = {{{2.0f, 1.0f}, 3.0f}, 4};
        line = {-1.0f, 2.0f, 4.0f};
        count = arc.intersection_points(line, point_1);
        EXPECT_EQ(count, IntersectionCount::NO_INTERSECTION);

        arc = {{{2.0f, 1.0f}, 3.0f}, 4};
        line = {-3.0f, -2.0f, 3.0f};
        count = arc.intersection_points(line, point_1);
        EXPECT_EQ(count, IntersectionCount::NO_INTERSECTION);

        arc = {{{2.0f, 1.0f}, 3.0f}, 4};
        line = {-3.0f, -0.5f, 3.0f};
        count = arc.intersection_points(line, point_1);
        EXPECT_EQ(count, IntersectionCount::ONE_POINT);
        EXPECT_NEAR_V2(point_1, glm::vec2(3.0f, -1.82842712474619f), THRESHOLD);

        arc = {{{2.0f, 1.0f}, 3.0f}, 4};
        line = {-3.5f, -0.5f, 2.0f};
        count = arc.intersection_points(line, point_1);
        EXPECT_EQ(count, IntersectionCount::ONE_POINT);
        EXPECT_NEAR_V2(point_1, glm::vec2(2.0f, -2.0f), THRESHOLD);

        arc = {{{2.0f, 1.0f}, 3.0f}, 4};
        line = {-3.0f, 2.0f, 5.0f};
        count = arc.intersection_points(line, point_1);
        EXPECT_EQ(count, IntersectionCount::ONE_POINT);
        EXPECT_NEAR_V2(point_1, glm::vec2(5.0f, 1.0f), THRESHOLD);
    }

    TEST(math_test, arc_and_hor_line_intersection_points) {
        glm::vec2 point_1;

        // {{{center.x, center.y}, radius}, quadrant}.
        QuadrantArc arc {{{1.0f, 2.0f}, 3.0f}, 2};
        // {min_y, max_y, x}.
        HorLine line {-1.0f, 3.0f, 2.0f};
        auto count {arc.intersection_points(line, point_1)};
        EXPECT_EQ(count, IntersectionCount::NO_INTERSECTION);

        arc = {{{1.0f, 2.0f}, 3.0f}, 2};
        line = {-1.0f, 2.0f, 4.0f};
        count = arc.intersection_points(line, point_1);
        EXPECT_EQ(count, IntersectionCount::NO_INTERSECTION);

        arc = {{{1.0f, 2.0f}, 3.0f}, 2};
        line = {-3.0f, -2.0f, 3.0f};
        count = arc.intersection_points(line, point_1);
        EXPECT_EQ(count, IntersectionCount::NO_INTERSECTION);

        arc = {{{1.0f, 2.0f}, 3.0f}, 2};
        line = {-3.0f, -0.5f, 3.0f};
        count = arc.intersection_points(line, point_1);
        EXPECT_EQ(count, IntersectionCount::ONE_POINT);
        EXPECT_NEAR_V2(point_1, glm::vec2(-1.82842712474619f, 3.0f), THRESHOLD);

        arc = {{{1.0f, 2.0f}, 3.0f}, 2};
        line = {-3.5f, -0.5f, 2.0f};
        count = arc.intersection_points(line, point_1);
        EXPECT_EQ(count, IntersectionCount::ONE_POINT);
        EXPECT_NEAR_V2(point_1, glm::vec2(-2.0f, 2.0f), THRESHOLD);

        arc = {{{1.0f, 2.0f}, 3.0f}, 2};
        line = {-3.0f, 2.0f, 5.0f};
        count = arc.intersection_points(line, point_1);
        EXPECT_EQ(count, IntersectionCount::ONE_POINT);
        EXPECT_NEAR_V2(point_1, glm::vec2(1.0f, 5.0f), THRESHOLD);
    };

    TEST(math_test, hor_and_vert_line_intersection_point) {
        glm::vec2 point_1;

        HorLine hor_line {-3.0f, 1.0f, -1.0f};
        VertLine vert_line {-2.0f, 3.0f, -1.0f};
        auto count {utils::vert_and_hor_line_intersection_points(vert_line, hor_line, point_1)};
        EXPECT_EQ(count, IntersectionCount::ONE_POINT);
        EXPECT_NEAR_V2(point_1, glm::vec2(-1.0f, -1.0f), THRESHOLD);

        hor_line = {-3.0f, 1.0f, -1.0f};
        vert_line = {-2.0f, 3.0f, 1.0f};
        count = utils::vert_and_hor_line_intersection_points(vert_line, hor_line, point_1);
        EXPECT_EQ(count, IntersectionCount::ONE_POINT);
        EXPECT_NEAR_V2(point_1, glm::vec2(1.0f, -1.0f), THRESHOLD);

        hor_line = {-3.0f, 1.0f, -1.0f};
        vert_line = {-2.0f, 0.0f, 2.0f};
        count = utils::vert_and_hor_line_intersection_points(vert_line, hor_line, point_1);
        EXPECT_EQ(count, IntersectionCount::NO_INTERSECTION);
    }

    TEST(math_test, two_hor_line_segments_intersection_points) {
        glm::vec2 point_1;

        HorLine line_1 {-1.0f, 2.0f, 1.0f};
        HorLine line_2 {1.0f, 3.0f, 0.0f};
        auto count {line_1.intersection_points(line_2, point_1, true)};
        EXPECT_EQ(count, IntersectionCount::NO_INTERSECTION);

        line_1 = {-1.0f, 2.0f, 1.0f};
        line_2 = {1.0f, 3.0f, 0.0f};
        count = line_1.intersection_points(line_2, point_1, false);
        EXPECT_EQ(count, IntersectionCount::NO_INTERSECTION);

        line_1 = {-1.0f, 2.0f, 1.0f};
        line_2 = {1.0f, 3.0f, 1.0f};
        count = line_1.intersection_points(line_2, point_1, true);
        EXPECT_EQ(count, IntersectionCount::INFINITE_POINTS);

        line_1 = {-1.0f, 2.0f, 1.0f};
        line_2 = {1.0f, 3.0f, 1.0f};
        count = line_1.intersection_points(line_2, point_1, false);
        EXPECT_EQ(count, IntersectionCount::INFINITE_POINTS);

        line_1 = {-1.0f, 2.0f, 1.0f};
        line_2 = {-2.0f, -1.0f, 1.0f};
        count = line_1.intersection_points(line_2, point_1, true);
        EXPECT_EQ(count, IntersectionCount::ONE_POINT);
        EXPECT_NEAR_V2(point_1, glm::vec2(-1.0f, 1.0f), THRESHOLD);

        line_1 = {-1.0f, 2.0f, 1.0f};
        line_2 = {-2.0f, -1.0f, 1.0f};
        count = line_1.intersection_points(line_2, point_1, false);
        EXPECT_EQ(count, IntersectionCount::NO_INTERSECTION);
    }

    TEST(math_test, two_vert_line_segments_intersection_points) {
        glm::vec2 point_1;

        VertLine line_1 {-1.0f, 2.0f, 1.0f};
        VertLine line_2 {1.0f, 3.0f, 0.0f};
        auto count {line_1.intersection_points(line_2, point_1, true)};
        EXPECT_EQ(count, IntersectionCount::NO_INTERSECTION);

        line_1 = {-1.0f, 2.0f, 1.0f};
        line_2 = {1.0f, 3.0f, 0.0f};
        count = line_1.intersection_points(line_2, point_1, false);
        EXPECT_EQ(count, IntersectionCount::NO_INTERSECTION);

        line_1 = {-1.0f, 2.0f, 1.0f};
        line_2 = {1.0f, 3.0f, 1.0f};
        count = line_1.intersection_points(line_2, point_1, true);
        EXPECT_EQ(count, IntersectionCount::INFINITE_POINTS);

        line_1 = {-1.0f, 2.0f, 1.0f};
        line_2 = {1.0f, 3.0f, 1.0f};
        count = line_1.intersection_points(line_2, point_1, false);
        EXPECT_EQ(count, IntersectionCount::INFINITE_POINTS);

        line_1 = {-1.0f, 2.0f, 1.0f};
        line_2 = {-2.0f, -1.0f, 1.0f};
        count = line_1.intersection_points(line_2, point_1, true);
        EXPECT_EQ(count, IntersectionCount::ONE_POINT);
        EXPECT_NEAR_V2(point_1, glm::vec2(1.0f, -1.0f), THRESHOLD);

        line_1 = {-1.0f, 2.0f, 1.0f};
        line_2 = {-2.0f, -1.0f, 1.0f};
        count = line_1.intersection_points(line_2, point_1, false);
        EXPECT_EQ(count, IntersectionCount::NO_INTERSECTION);
    }
}
