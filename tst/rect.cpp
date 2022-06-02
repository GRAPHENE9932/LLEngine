#include <gtest/gtest.h>

#include "structs/Rect.hpp"

namespace {
    constexpr float THRESHOLD = 0.0001f;

    TEST(math_test, rect_contains) {
        Rect rect_1({-6.0f, -4.0f}, {8.0f, 16.0f});
        EXPECT_EQ(rect_1.contains({-8.0f, 14.0f}), false);
        EXPECT_EQ(rect_1.contains({-5.0f, 6.0f}), true);
        EXPECT_EQ(rect_1.contains({1.0f, 11.0f}), true);
        EXPECT_EQ(rect_1.contains({3.0f, -2.0f}), false);
        EXPECT_EQ(rect_1.contains({-4.0f, -4.0f}), true);

        Rect rect_2({15.0f, 0.0f}, {7.0f, 3.0f});
        EXPECT_EQ(rect_2.contains({16.0f, 4.0f}), false);
        EXPECT_EQ(rect_2.contains({17.0f, 2.0f}), true);
        EXPECT_EQ(rect_2.contains({21.0f, 1.0f}), true);
        EXPECT_EQ(rect_2.contains({23.0f, 1.0f}), false);
        EXPECT_EQ(rect_2.contains({15.0f, 3.0f}), true);
    }

    TEST(math_test, rect_distance_from) {
        Rect rect_1({-6.0f, -4.0f}, {8.0f, 16.0f});
        EXPECT_NEAR(rect_1.distance_from({-8.0f, 14.0f}), 2.0f * M_SQRT2, THRESHOLD);
        EXPECT_NEAR(rect_1.distance_from({-5.0f, 6.0f}), 0.0f, THRESHOLD);
        EXPECT_NEAR(rect_1.distance_from({1.0f, 11.0f}), 0.0f, THRESHOLD);
        EXPECT_NEAR(rect_1.distance_from({3.0f, -2.0f}), 1.0f, THRESHOLD);
        EXPECT_NEAR(rect_1.distance_from({-4.0f, -4.0f}), 0.0f, THRESHOLD);

        Rect rect_2({15.0f, 0.0f}, {7.0f, 3.0f});
        EXPECT_NEAR(rect_2.distance_from({16.0f, 4.0f}), 1.0f, THRESHOLD);
        EXPECT_NEAR(rect_2.distance_from({17.0f, 2.0f}), 0.0f, THRESHOLD);
        EXPECT_NEAR(rect_2.distance_from({21.0f, 1.0f}), 0.0f, THRESHOLD);
        EXPECT_NEAR(rect_2.distance_from({23.0f, 1.0f}), 1.0f, THRESHOLD);
        EXPECT_NEAR(rect_2.distance_from({15.0f, 3.0f}), 0.0f, THRESHOLD);
    }
}
