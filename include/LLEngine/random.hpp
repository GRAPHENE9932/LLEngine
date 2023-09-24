#pragma once

#include <cstdint>

namespace llengine::random {
[[nodiscard]] float float_in_range(float min, float max);
[[nodiscard]] float float_gaussian_distribution(float mean, float standard_deviation);
[[nodiscard]] std::int32_t int32_in_range(std::int32_t min, std::int32_t max);
[[nodiscard]] bool generate_bool();
}