#include <gtest/gtest.h>

#include "structs/Circle.hpp"
#include "structs/HorLS.hpp"
#include "structs/VertLS.hpp"
#include "structs/QuadrantArc.hpp"
#include "structs/RoundedRectangle.hpp"
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
        EXPECT_EQ(count, 2);
        EXPECT_NEAR_2_UNORDERED_V2(point_1, point_2, glm::vec2(0.0f, 2.0f), glm::vec2(2.0f, 0.0f), THRESHOLD);

        circle_1 = {{0.0f, 0.0f}, 2.0f};
        circle_2 = {{-2.0f, 3.0f}, 2.0f};
        count = circle_1.intersection_points(circle_2, point_1, point_2);
        EXPECT_EQ(count, 2);
        EXPECT_NEAR_2_UNORDERED_V2(
            point_1, point_2,
            glm::vec2(-1.720576692122892f, 1.019615538584738f), glm::vec2(-0.279423307877108f, 1.980384461415261f),
            THRESHOLD
        );

        circle_1 = {{1.0f, 1.0f}, 2.0f};
        circle_2 = {{1.0f, 3.0f}, 2.0f};
        count = circle_1.intersection_points(circle_2, point_1, point_2);
        EXPECT_EQ(count, 2);
        EXPECT_NEAR_2_UNORDERED_V2(
            point_1, point_2,
            glm::vec2(-0.732050807568877f, 2.0f), glm::vec2(2.732050807568877f, 2.0f),
            THRESHOLD
        );

        circle_1 = {{5.0f, 4.0f}, 2.0f};
        circle_2 = {{1.0f, 3.0f}, 2.0f};
        count = circle_1.intersection_points(circle_2, point_1, point_2);
        EXPECT_EQ(count, 0);

        circle_1 = {{1.0f, 3.0f}, 2.0f};
        circle_2 = {{1.0f, 3.0f}, 2.0f};
        count = circle_1.intersection_points(circle_2, point_1, point_2);
        EXPECT_EQ(count, INFINITE_POINTS);
    }

    TEST(math_test, two_arcs_intersection_points) {
        glm::vec2 point_1, point_2;

        // {{{center.x, center.y}, radius}, quadrant}.
        QuadrantArc arc_1 {{{0.0f, 0.0f}, 2.0f}, 1};
        QuadrantArc arc_2 {{{2.0f, 3.0f}, 2.0f}, 3};
        auto count {arc_1.intersection_points(arc_2, point_1, point_2)};
        EXPECT_EQ(count, 2);
        EXPECT_NEAR_2_UNORDERED_V2(
            point_1, point_2,
            glm::vec2(0.279423307877108f, 1.980384461415261f), glm::vec2(1.720576692122892f, 1.019615538584738f),
            THRESHOLD
        );
        
        arc_1 = {{{0.0f, 0.0f}, 2.0f}, 1};
        arc_2 = {{{1.5f, 3.0f}, 2.0f}, 3};
        count = arc_1.intersection_points(arc_2, point_1, point_2);
        EXPECT_EQ(count, 0);
        
        arc_1 = {{{-1.0f, 4.0f}, 2.0f}, 1};
        arc_2 = {{{1.5f, 3.0f}, 2.0f}, 2};
        count = arc_1.intersection_points(arc_2, point_1, point_2);
        EXPECT_EQ(count, 1);
        EXPECT_NEAR_V2(
            point_1, glm::vec2(0.799294218002552f, 4.873235545006379f), THRESHOLD
        );

        arc_1 = {{{-1.0f, 4.0f}, 2.0f}, 2};
        arc_2 = {{{-1.0f, 4.0f}, 2.0f}, 2};
        count = arc_1.intersection_points(arc_2, point_1, point_2);
        EXPECT_EQ(count, INFINITE_POINTS);
        
        arc_1 = {{{-1.0f, 4.0f}, 2.0f}, 2};
        arc_2 = {{{-1.0f, 4.0f}, 2.0f}, 4};
        count = arc_1.intersection_points(arc_2, point_1, point_2);
        EXPECT_EQ(count, 0);

        arc_1 = {{{-1.0f, 4.0f}, 2.0f}, 2};
        arc_2 = {{{-1.0f, 4.0f}, 2.0f}, 3};
        count = arc_1.intersection_points(arc_2, point_1, point_2);
        EXPECT_EQ(count, 1);
        EXPECT_NEAR_V2(point_1, glm::vec2(-3.0f, 4.0), THRESHOLD);
    }

    TEST(math_test, arc_and_vert_line_intersection_points) {
        glm::vec2 point_1;

        // {{{center.x, center.y}, radius}, quadrant}.
        QuadrantArc arc {{{2.0f, 1.0f}, 3.0f}, 4};
        // {min_x, max_x, y}.
        VertLS line {-1.0f, 5.0f, 2.0f};
        auto count {arc.intersection_points(line, point_1)};
        EXPECT_EQ(count, 0);
        
        arc = {{{2.0f, 1.0f}, 3.0f}, 4};
        line = {-1.0f, 2.0f, 4.0f};
        count = arc.intersection_points(line, point_1);
        EXPECT_EQ(count, 0);

        arc = {{{2.0f, 1.0f}, 3.0f}, 4};
        line = {-3.0f, -2.0f, 3.0f};
        count = arc.intersection_points(line, point_1);
        EXPECT_EQ(count, 0);

        arc = {{{2.0f, 1.0f}, 3.0f}, 4};
        line = {-3.0f, -0.5f, 3.0f};
        count = arc.intersection_points(line, point_1);
        EXPECT_EQ(count, 1);
        EXPECT_NEAR_V2(point_1, glm::vec2(3.0f, -1.82842712474619f), THRESHOLD);

        arc = {{{2.0f, 1.0f}, 3.0f}, 4};
        line = {-3.5f, -0.5f, 2.0f};
        count = arc.intersection_points(line, point_1);
        EXPECT_EQ(count, 1);
        EXPECT_NEAR_V2(point_1, glm::vec2(2.0f, -2.0f), THRESHOLD);

        arc = {{{2.0f, 1.0f}, 3.0f}, 4};
        line = {-3.0f, 2.0f, 5.0f};
        count = arc.intersection_points(line, point_1);
        EXPECT_EQ(count, 1);
        EXPECT_NEAR_V2(point_1, glm::vec2(5.0f, 1.0f), THRESHOLD);
    }

    TEST(math_test, arc_and_hor_line_intersection_points) {
        glm::vec2 point_1;

        // {{{center.x, center.y}, radius}, quadrant}.
        QuadrantArc arc {{{1.0f, 2.0f}, 3.0f}, 2};
        // {min_y, max_y, x}.
        HorLS line {-1.0f, 3.0f, 2.0f};
        auto count {arc.intersection_points(line, point_1)};
        EXPECT_EQ(count, 0);

        arc = {{{1.0f, 2.0f}, 3.0f}, 2};
        line = {-1.0f, 2.0f, 4.0f};
        count = arc.intersection_points(line, point_1);
        EXPECT_EQ(count, 0);

        arc = {{{1.0f, 2.0f}, 3.0f}, 2};
        line = {-3.0f, -2.0f, 3.0f};
        count = arc.intersection_points(line, point_1);
        EXPECT_EQ(count, 0);

        arc = {{{1.0f, 2.0f}, 3.0f}, 2};
        line = {-3.0f, -0.5f, 3.0f};
        count = arc.intersection_points(line, point_1);
        EXPECT_EQ(count, 1);
        EXPECT_NEAR_V2(point_1, glm::vec2(-1.82842712474619f, 3.0f), THRESHOLD);

        arc = {{{1.0f, 2.0f}, 3.0f}, 2};
        line = {-3.5f, -0.5f, 2.0f};
        count = arc.intersection_points(line, point_1);
        EXPECT_EQ(count, 1);
        EXPECT_NEAR_V2(point_1, glm::vec2(-2.0f, 2.0f), THRESHOLD);

        arc = {{{1.0f, 2.0f}, 3.0f}, 2};
        line = {-3.0f, 2.0f, 5.0f};
        count = arc.intersection_points(line, point_1);
        EXPECT_EQ(count, 1);
        EXPECT_NEAR_V2(point_1, glm::vec2(1.0f, 5.0f), THRESHOLD);
    };

    TEST(math_test, hor_and_vert_line_intersection_point) {
        glm::vec2 point_1;

        HorLS hor_line {-3.0f, 1.0f, -1.0f};
        VertLS vert_line {-2.0f, 3.0f, -1.0f};
        auto count {vert_line.intersection_points(hor_line, point_1)};
        EXPECT_EQ(count, 1);
        EXPECT_NEAR_V2(point_1, glm::vec2(-1.0f, -1.0f), THRESHOLD);

        hor_line = {-3.0f, 1.0f, -1.0f};
        vert_line = {-2.0f, 3.0f, 1.0f};
        count = vert_line.intersection_points(hor_line, point_1);
        EXPECT_EQ(count, 1);
        EXPECT_NEAR_V2(point_1, glm::vec2(1.0f, -1.0f), THRESHOLD);

        hor_line = {-3.0f, 1.0f, -1.0f};
        vert_line = {-2.0f, 0.0f, 2.0f};
        count = vert_line.intersection_points(hor_line, point_1);
        EXPECT_EQ(count, 0);
    }

    TEST(math_test, two_hor_line_segments_intersection_points) {
        glm::vec2 point_1;

        HorLS line_1 {-1.0f, 2.0f, 1.0f};
        HorLS line_2 {1.0f, 3.0f, 0.0f};
        auto count {line_1.intersection_points(line_2, point_1, true)};
        EXPECT_EQ(count, 0);

        line_1 = {-1.0f, 2.0f, 1.0f};
        line_2 = {1.0f, 3.0f, 0.0f};
        count = line_1.intersection_points(line_2, point_1, false);
        EXPECT_EQ(count, 0);

        line_1 = {-1.0f, 2.0f, 1.0f};
        line_2 = {1.0f, 3.0f, 1.0f};
        count = line_1.intersection_points(line_2, point_1, true);
        EXPECT_EQ(count, INFINITE_POINTS);

        line_1 = {-1.0f, 2.0f, 1.0f};
        line_2 = {1.0f, 3.0f, 1.0f};
        count = line_1.intersection_points(line_2, point_1, false);
        EXPECT_EQ(count, INFINITE_POINTS);

        line_1 = {-1.0f, 2.0f, 1.0f};
        line_2 = {-2.0f, -1.0f, 1.0f};
        count = line_1.intersection_points(line_2, point_1, true);
        EXPECT_EQ(count, 1);
        EXPECT_NEAR_V2(point_1, glm::vec2(-1.0f, 1.0f), THRESHOLD);

        line_1 = {-1.0f, 2.0f, 1.0f};
        line_2 = {-2.0f, -1.0f, 1.0f};
        count = line_1.intersection_points(line_2, point_1, false);
        EXPECT_EQ(count, 0);
    }

    TEST(math_test, two_vert_line_segments_intersection_points) {
        glm::vec2 point_1;

        VertLS line_1 {-1.0f, 2.0f, 1.0f};
        VertLS line_2 {1.0f, 3.0f, 0.0f};
        auto count {line_1.intersection_points(line_2, point_1, true)};
        EXPECT_EQ(count, 0);

        line_1 = {-1.0f, 2.0f, 1.0f};
        line_2 = {1.0f, 3.0f, 0.0f};
        count = line_1.intersection_points(line_2, point_1, false);
        EXPECT_EQ(count, 0);

        line_1 = {-1.0f, 2.0f, 1.0f};
        line_2 = {1.0f, 3.0f, 1.0f};
        count = line_1.intersection_points(line_2, point_1, true);
        EXPECT_EQ(count, INFINITE_POINTS);

        line_1 = {-1.0f, 2.0f, 1.0f};
        line_2 = {1.0f, 3.0f, 1.0f};
        count = line_1.intersection_points(line_2, point_1, false);
        EXPECT_EQ(count, INFINITE_POINTS);

        line_1 = {-1.0f, 2.0f, 1.0f};
        line_2 = {-2.0f, -1.0f, 1.0f};
        count = line_1.intersection_points(line_2, point_1, true);
        EXPECT_EQ(count, 1);
        EXPECT_NEAR_V2(point_1, glm::vec2(1.0f, -1.0f), THRESHOLD);

        line_1 = {-1.0f, 2.0f, 1.0f};
        line_2 = {-2.0f, -1.0f, 1.0f};
        count = line_1.intersection_points(line_2, point_1, false);
        EXPECT_EQ(count, 0);
    }

    TEST(math_test, two_rounded_rectangles_intersection_points) {
        std::array<glm::vec2, 4> points = {{{1.1f, 2.0f}, {3.0f, 4.0f}}};

        RoundedRectangle rect_1({{-4.0f, -2.0f}, {6.0f, 10.0f}}, 2.0f);
        RoundedRectangle rect_2({{4.0f, -2.0f}, {6.0f, 10.0f}}, 2.0f);
        auto count {rect_1.intersection_points(rect_2, points)};
        EXPECT_EQ(count, 2);
        std::array<glm::vec2, 2> correct_2 {{{3.0f, -3.732050807568877f}, {3.0f, 9.732050807568877f}}};
        EXPECT_NEAR_UNORDERED_V2_ARRAY(points, correct_2, 2, THRESHOLD);

        rect_1 = {{{-4.0f, -2.0f}, {6.0f, 10.0f}}, 2.0f};
        rect_2 = {{{-1.0f, 4.0f}, {4.0f, 10.0f}}, 2.0f};
        count = rect_1.intersection_points(rect_2, points);
        EXPECT_EQ(count, 2);
        correct_2 = {{{-3.0f, 10.0f}, {4.0f, 2.267949192431122f}}};
        EXPECT_NEAR_UNORDERED_V2_ARRAY(points, correct_2, 2, THRESHOLD);

        rect_1 = {{{-4.0f, -2.0f}, {6.0f, 10.0f}}, 2.0f};
        rect_2 = {{{-11.0f, -7.0f}, {4.0f, 10.0f}}, 2.0f};
        count = rect_1.intersection_points(rect_2, points);
        EXPECT_EQ(count, 2);
        correct_2 = {{{-6.0f, 4.732050807568878f}, {-5.0f, -3.732050807568878f}}};
        EXPECT_NEAR_UNORDERED_V2_ARRAY(points, correct_2, 2, THRESHOLD);

        rect_1 = {{{-4.0f, -2.0f}, {6.0f, 10.0f}}, 2.0f};
        rect_2 = {{{-10.0f, 2.0f}, {18.0f, 4.0f}}, 2.0f};
        count = rect_1.intersection_points(rect_2, points);
        EXPECT_EQ(count, 4);
        std::array<glm::vec2, 4> correct_4 {{{-6.0f, 8.0f}, {4.0f, 8.0f}, {-6.0f, 0.0f}, {4.0f, 0.0f}}};
        EXPECT_NEAR_UNORDERED_V2_ARRAY(points, correct_4, 4, THRESHOLD);

        rect_1 = {{{-4.0f, -2.0f}, {6.0f, 10.0f}}, 2.0f};
        rect_2 = {{{-14.0f, 10.0f}, {7.0f, 3.0f}}, 2.0f};
        count = rect_1.intersection_points(rect_2, points);
        EXPECT_EQ(count, 2);
        correct_2 = {{{-5.980384461415261f, 8.279423307877108f}, {-5.019615538584739f, 9.720576692122892f}}};
        EXPECT_NEAR_UNORDERED_V2_ARRAY(points, correct_2, 2, THRESHOLD);

        rect_1 = {{{-4.0f, -2.0f}, {6.0f, 10.0f}}, 2.0f};
        rect_2 = {{{-14.0f, 11.0f}, {4.0f, 3.0f}}, 2.0f};
        count = rect_1.intersection_points(rect_2, points);
        EXPECT_EQ(count, 0);
    }
}
