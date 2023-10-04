#pragma once

#include "Key.hpp"

#include <glm/vec2.hpp> // glm::ivec2

#include <vector>
#include <string_view> // std::string_view

struct GLFWwindow;

namespace llengine {
class Node;

class GLFWWindow {
public:
    enum KeyEventType : std::uint8_t {
        PRESS_EVENT, RELEASE_EVENT
    };

    GLFWWindow(glm::ivec2 extents, std::string_view title,
               int gl_version_major, int gl_version_minor);
    GLFWWindow(const GLFWWindow& other) = delete;
    GLFWWindow(GLFWWindow&& other) = delete;
    ~GLFWWindow();

    GLFWWindow& operator=(const GLFWWindow& other) = delete;
    GLFWWindow& operator=(GLFWWindow&& other) = delete;

    void swap_buffers();
    [[nodiscard]] bool window_should_close() const;
    void disable_cursor();
    void enable_cursor();
    [[nodiscard]] glm::dvec2 get_cursor_position() const;
    [[nodiscard]] glm::ivec2 get_window_size() const;
    [[nodiscard]] glm::ivec2 get_framebuffer_size() const;
    void set_cursor_position(const glm::dvec2 & new_position);
    [[nodiscard]] bool is_key_pressed(Key key) const;
    [[nodiscard]] bool is_mouse_button_pressed(std::uint8_t button) const;

private:
    // This static count is needed to terminate GLFW when
    // all windows are closed.
    static size_t glfw_windows_count;

    GLFWwindow* glfw_window;
    std::vector<std::uint8_t> pressed_mouse_buttons;
    std::vector<std::reference_wrapper<Node>> keyboard_key_event_subscribers;

    void subscribe_to_keyboard_key_event(Node& node);
    void unsubscribe_from_keyboard_key_event(Node& node);
    void call_keyboard_key_event_subscribers(Key key, KeyEventType event_type);

    static void keyboard_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

    friend class Node;
};
}