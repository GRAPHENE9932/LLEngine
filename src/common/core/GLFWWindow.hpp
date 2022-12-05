#pragma once

#include "IWindow.hpp" // IWindow

#include <glm/vec2.hpp> // glm::ivec2

#include <string_view> // std::string_view

class GLFWwindow;

class GLFWWindow : public IWindow {
public:
    GLFWWindow(glm::ivec2 extents, std::string_view title,
               int gl_version_major, int gl_version_minor);
    ~GLFWWindow();

    void swap_buffers() override;
    bool window_should_close() const override;
    void disable_cursor() override;
    glm::dvec2 get_cursor_position() const override;
    glm::ivec2 get_window_size() const override;
    void set_cursor_position(const glm::dvec2 & new_position) override;
    bool is_key_pressed(Key key) const override;

private:
    // This static count is needed to terminate GLFW when
    // all windows are closed.
    static size_t glfw_windows_count;

    GLFWwindow* glfw_window;
};
