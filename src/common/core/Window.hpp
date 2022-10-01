#pragma once

#include <vector> // std::vector
#include <stdexcept> // std::runtime_error

#include <glm/vec2.hpp> // glm::ivec2
#include <GLFW/glfw3.h> // GLFWwindow

class Window {
public:
    static size_t windows_count;

    void initialize(glm::ivec2 extents, const char* title,
                    int gl_version_major, int gl_version_minor);
    ~Window();

    glm::ivec2 get_window_size() const;

    GLFWwindow* get() const {
        if (!window)
            throw std::runtime_error("The window is not initialized.");

        return window;
    }

    bool is_initialized() const noexcept {
        return window;
    }

private:
    GLFWwindow* window = nullptr;
};
