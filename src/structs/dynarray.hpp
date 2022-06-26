#pragma once

#include <cstddef>

template<typename T>
class dynarray {
public:
    dynarray(const std::size_t size) : size_impl(size) {
        data_ptr = new T[size];
    }

    template<typename... Args>
    dynarray(const std::size_t size, Args&&... args) : size_impl(size) {
        data_ptr = new T[size];
        for (std::size_t i = 0; i < size; i++)
            data_ptr[i] = T(args...);
    }

    ~dynarray() {
        delete[] data_ptr;
    }

    inline const T& operator[](const std::size_t index) const {
        return data_ptr[index];
    }
    inline T& operator[](const std::size_t index) {
        return data_ptr[index];
    }

    inline std::size_t size() const {
        return size_impl;
    }

    inline T* data() const {
        return data_ptr;
    }

private:
    std::size_t size_impl;
    T* data_ptr;
};
