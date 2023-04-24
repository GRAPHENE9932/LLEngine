#pragma once

#include "utils/key_codes.hpp"

#include <glm/vec2.hpp> // glm::ivec2

#include <vector>
#include <string_view> // std::string_view

class GLFWwindow;

class GLFWWindow {
public:
    GLFWWindow(glm::ivec2 extents, std::string_view title,
               int gl_version_major, int gl_version_minor);
    ~GLFWWindow();

    void swap_buffers();
    [[nodiscard]] bool window_should_close() const;
    void disable_cursor();
    [[nodiscard]] glm::dvec2 get_cursor_position() const;
    [[nodiscard]] glm::ivec2 get_window_size() const;
    void set_cursor_position(const glm::dvec2 & new_position);
    [[nodiscard]] bool is_key_pressed(Key key) const;
    [[nodiscard]] bool is_mouse_button_pressed(std::uint8_t button) const;

private:
    // This static count is needed to terminate GLFW when
    // all windows are closed.
    static size_t glfw_windows_count;

    GLFWwindow* glfw_window;
    std::vector<std::uint8_t> pressed_mouse_buttons;

    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
};
