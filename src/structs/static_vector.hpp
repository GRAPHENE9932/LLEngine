#pragma once

#include <array>
#include <stdexcept>

template<typename T, std::size_t C>
class static_vector {
public:
    inline void push_back(T object) noexcept {
        assert(cur_size < C);

        array[cur_size++] = object;
    }

    inline void empty() noexcept {
        cur_size = 0;
    }

    inline T& operator[](std::size_t index) noexcept {
        assert(index < cur_size);

        return array[index];
    }

    inline const T& operator[](std::size_t index) const noexcept {
        assert(index < cur_size);

        return array[index];
    }

    inline T& at(std::size_t index) const {
        if (index >= cur_size)
            throw std::out_of_range();
        
        return array[index];
    }

    inline std::size_t size() const noexcept {
        return cur_size;
    }

    inline const std::array<T, C>& get_array() const noexcept {
        return array;
    }

private:
    size_t cur_size {0};
    std::array<T, C> array;
};
