#include <limits>

#include <gtest/gtest.h>
#include <glm/trigonometric.hpp>

#include "structs/HorLine.hpp"
#include "structs/VertLine.hpp"
#include "structs/QuadrantArc.hpp"
#include "structs/RoundedRectangle.hpp"
#include "utils/math.hpp"
#include "macros.hpp"

namespace {
    constexpr float THRESHOLD = 0.0001f;

    TEST(math_test, closest_point_to_hor_line_segment) {
        // {lower_x, higher_x, y}.
        HorLine hor_line {0.0f, 2.0f, 1.0f};
        auto result = hor_line.closest_point({1.0f, 2.0f});
        EXPECT_EQ(result, glm::vec2(1.0f, 1.0f));

        hor_line = {-5.0f, -1.0f, -4.0f};
        result = hor_line.closest_point({-4.0f, -5.0f});
        EXPECT_EQ(result, glm::vec2(-4.0f, -4.0f));

        hor_line = {-1.0f, 2.0f, -2.0f};
        result = hor_line.closest_point({-8.0f, -2.0f});
        EXPECT_EQ(result, glm::vec2(-1.0f, -2.0f));

        hor_line = {-1.0f, 2.0f, -2.0f};
        result = hor_line.closest_point({-2.0f, -4.0f});
        EXPECT_EQ(result, glm::vec2(-1.0f, -2.0f));

        hor_line = {-1.0f, 2.0f, -2.0f};
        result = hor_line.closest_point({3.0f, -1.0f});
        EXPECT_EQ(result, glm::vec2(2.0f, -2.0f));
    }

    TEST(math_test, closest_point_to_ver_line_segment) {
        // {lower_y, higher_y, x}.
        VertLine vert_line {-2.0f, 1.0f, -1.0f};
        auto result = vert_line.closest_point({3.0f, -1.0f});
        EXPECT_NEAR_V2(result, glm::vec2(-1.0f, -1.0f), THRESHOLD);

        vert_line = {-2.0f, 1.0f, -1.0f};
        result = vert_line.closest_point({-2.0f, 0.0f});
        EXPECT_NEAR_V2(result, glm::vec2(-1.0f, 0.0f), THRESHOLD);

        vert_line = {-2.0f, 1.0f, -1.0f};
        result = vert_line.closest_point({-1.0f, 3.0f});
        EXPECT_NEAR_V2(result, glm::vec2(-1.0f, 1.0f), THRESHOLD);

        vert_line = {-2.0f, 1.0f, -1.0f};
        result = vert_line.closest_point({0.0f, -4.0f});
        EXPECT_NEAR_V2(result, glm::vec2(-1.0f, -2.0f), THRESHOLD);

        vert_line = {-2.0f, 1.0f, -1.0f};
        result = vert_line.closest_point({-1.0f, -1.0f});
        EXPECT_NEAR_V2(result, glm::vec2(-1.0f, -1.0f), THRESHOLD);
    }

    TEST(math_test, closest_point_to_arc) {
        // {{{center.x, center.y}, radius}, quadrant}.
        QuadrantArc arc {{{4.0f, -3.0f}, 2.0f}, 3};
        auto result = arc.closest_point({2.0f, -6.0f});
        EXPECT_NEAR_V2(result, glm::vec2(2.89059960755f, -4.664100588676), THRESHOLD);

        arc = {{{4.0f, -3.0f}, 2.0f}, 3};
        result = arc.closest_point({3.5f, -6.0f});
        EXPECT_NEAR_V2(result, glm::vec2(3.671202025389f, -4.972787847664f), THRESHOLD);

        arc = {{{4.0f, -3.0f}, 2.0f}, 3};
        result = arc.closest_point({3.83333333f, -4.0f});
        EXPECT_NEAR_V2(result, glm::vec2(3.671202025389f, -4.972787847664f), THRESHOLD);

        arc = {{{4.0f, -3.0f}, 2.0f}, 3};
        result = arc.closest_point({5.5f, -5.5f});
        EXPECT_NEAR_V2(result, glm::vec2(4.0f, -5.0f), THRESHOLD);

        arc = {{{4.0f, -3.0f}, 2.0f}, 3};
        result = arc.closest_point({3.5f, -2.5f});
        EXPECT_NEAR_V2(result, glm::vec2(2.0f, -3.0f), THRESHOLD);
    }

    TEST(math_test, closest_point_to_rounded_rectangle) {
        float distance { std::numeric_limits<float>().quiet_NaN() };

        RoundedRectangle rect {{{-10.0f, -10.0f}, {18.0f, 20.0f}}, 4.0f};
        auto result = rect.closest_point({-2.0f, 12.0f}, &distance);
        EXPECT_NEAR_V2(result, glm::vec2(-2.0f, 14.0f), THRESHOLD);
        EXPECT_NEAR(distance, 2.0f, THRESHOLD);

        distance = std::numeric_limits<float>().quiet_NaN();
        rect = {{{-10.0f, -10.0f}, {18.0f, 20.0f}}, 4.0f};
        result = rect.closest_point({-11.0f, 8.0f}, &distance);
        EXPECT_NEAR_V2(result, glm::vec2(-14.0f, 8.0f), THRESHOLD);
        EXPECT_NEAR(distance, 3.0f, THRESHOLD);

        distance = std::numeric_limits<float>().quiet_NaN();
        rect = {{{-10.0f, -10.0f}, {18.0f, 20.0f}}, 4.0f};
        result = rect.closest_point({12.0f, -4.0f}, &distance);
        EXPECT_NEAR_V2(result, glm::vec2(12.0f, -4.0f), THRESHOLD);
        EXPECT_NEAR(distance, 0.0f, THRESHOLD);

        distance = std::numeric_limits<float>().quiet_NaN();
        rect = {{{-10.0f, -10.0f}, {18.0f, 20.0f}}, 4.0f};
        result = rect.closest_point({-11.0f, -11.0f}, &distance);
        EXPECT_NEAR_V2(result, glm::vec2(-12.82842712474619f, -12.82842712474619f), THRESHOLD);
        EXPECT_NEAR(distance, 2.585786438f, THRESHOLD);

        distance = std::numeric_limits<float>().quiet_NaN();
        rect = {{{-10.0f, -10.0f}, {18.0f, 20.0f}}, 4.0f};
        result = rect.closest_point({6.0f, 9.0f}, &distance);
        EXPECT_NEAR_V2(result, glm::vec2(6.0f, 14.0f), THRESHOLD);
        EXPECT_NEAR(distance, 5.0f, THRESHOLD);

        distance = std::numeric_limits<float>().quiet_NaN();
        rect = {{{-10.0f, -10.0f}, {18.0f, 20.0f}}, 4.0f};
        result = rect.closest_point({9.0f, 12.0f}, &distance);
        EXPECT_NEAR_V2(result, glm::vec2(9.788854381999831f, 13.577708763999663f), THRESHOLD);
        EXPECT_NEAR(distance, 1.76393202250021f, THRESHOLD);

        rect = {{{-10.0f, -10.0f}, {18.0f, 20.0f}}, 4.0f};
        result = rect.closest_point({-2.0f, 12.0f}, nullptr);
        EXPECT_NEAR_V2(result, glm::vec2(-2.0f, 14.0f), THRESHOLD);
    }
}
