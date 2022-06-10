#include <cstring>
#include <string>
#include <sstream>
#include <iostream>

#define EXPECT_NEAR_V2(vec_1, vec_2, error_abs)\
    EXPECT_NEAR(vec_1.x, vec_2.x, error_abs);\
    EXPECT_NEAR(vec_1.y, vec_2.y, error_abs);

#define IS_NEAR(value_1, value_2, error_abs)\
    std::abs(value_1 - value_2) <= error_abs

#define IS_NEAR_V2(vec_1, vec_2, error_abs)\
    IS_NEAR(vec_1.x, vec_2.x, error_abs) && IS_NEAR(vec_1.y, vec_2.y, error_abs)

#define EXPECT_NEAR_2_UNORDERED_V2(vec_1, vec_2, vec_1_cor, vec_2_cor, error_abs)\
    EXPECT_TRUE((IS_NEAR_V2(vec_1, vec_1_cor, error_abs) && IS_NEAR_V2(vec_2, vec_2_cor, error_abs)) ||\
        (IS_NEAR_V2(vec_2, vec_1_cor, error_abs) && IS_NEAR_V2(vec_1, vec_2_cor, error_abs))) <<\
    "Unordered 2 vec2 does not match. Given vectors:\n" <<\
    '(' << vec_1.x << "; " << vec_1.y << ")\n" <<\
    '(' << vec_2.x << "; " << vec_2.y << ")\n" <<\
    "Correct vectors:\n" <<\
    '(' << vec_1_cor.x << "; " << vec_1_cor.y << ")\n" <<\
    '(' << vec_2_cor.x << "; " << vec_2_cor.y << ")\n" <<\
    "Acceptable absolute error:\n" <<\
    error_abs;

template<size_t S>
std::string array_to_string(const std::array<glm::vec2, S>& array) {
    std::stringstream ss;
    for (size_t i = 0; i < S; i++)
        ss << "(" << array[i].x << ", " << array[i].y << "),\n";
    return ss.str();
}

#define EXPECT_NEAR_UNORDERED_V2_ARRAY(arr_1, arr_2, size, error_abs)\
    std::sort(arr_1.begin(), arr_1.begin() + size, [](const glm::vec2& vec_1, const glm::vec2& vec_2) {\
        return std::memcmp(&vec_1, &vec_2, sizeof(glm::vec2)) < 0;\
    });\
    std::sort(arr_2.begin(), arr_2.begin() + size, [](const glm::vec2& vec_1, const glm::vec2& vec_2) {\
        return std::memcmp(&vec_1, &vec_2, sizeof(glm::vec2)) < 0;\
    });\
    for (int tmp_idx = 0; tmp_idx < size; tmp_idx++) {\
        EXPECT_TRUE(IS_NEAR_V2(arr_1[tmp_idx], arr_2[tmp_idx], error_abs)) <<\
            "Unordered arrays with size " << size << " does not match. Given array:\n" <<\
            array_to_string(arr_1) << "Correct array:\n" << array_to_string(arr_2);\
        EXPECT_NEAR_V2(arr_1[tmp_idx], arr_2[tmp_idx], error_abs);\
    }
