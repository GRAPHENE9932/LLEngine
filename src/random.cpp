#include "random.hpp"

#include <random>
#include <optional>

namespace llengine::random {
class LazyRandomizer {
public:
    std::mt19937* operator->() {
        initialize_randomizer_if_needed();
        return &randomizer.value();
    }

    std::mt19937::result_type operator()() {
        initialize_randomizer_if_needed();
        return (*randomizer)();
    }

    operator std::mt19937&() {
        initialize_randomizer_if_needed();
        return *randomizer;
    }

    std::mt19937& get() {
        initialize_randomizer_if_needed();
        return *randomizer;
    }
private:
    std::optional<std::mt19937> randomizer = std::nullopt;

    void initialize_randomizer_if_needed() {
        if (!randomizer.has_value()) {
            std::random_device device;
            randomizer = std::make_optional<std::mt19937>(device());
        }
    }
};
static LazyRandomizer randomizer;

[[nodiscard]] float float_in_range(float min, float max) {
    std::uniform_real_distribution<float> distribution(min, max);
    return distribution(randomizer.get());
}

[[nodiscard]] float float_gaussian_distribution(float mean, float standard_deviation) {
    std::normal_distribution<float> distribution(mean, standard_deviation);
    return distribution(randomizer.get());
}

[[nodiscard]] std::int32_t int32_in_range(std::int32_t min, std::int32_t max) {
    std::uniform_int_distribution<std::int32_t> distribution(min, max);
    return distribution(randomizer.get());
}

[[nodiscard]] bool generate_bool() {
    return static_cast<bool>(int32_in_range(0, 1));
}
}