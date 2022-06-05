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
    