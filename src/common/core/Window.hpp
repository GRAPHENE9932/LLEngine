#pragma once

#include <glm/vec2.hpp> // glm::ivec2, glm::dvec2

struct GLFWwindow;

class Window {
public:
    static size_t windows_count;

    void initialize(glm::ivec2 extents, const char* title,
                    int gl_version_major, int gl_version_minor);
    ~Window();

    /**
     * @brief Calls the glfwSwapBuffers function
     * on the window.
     *
     * @throws uninitialized_error if window is unintialized.
     */
    void glfw_swap_buffers();
    /**
     * @brief Checks if the window should close (for example,
     * when the close button is clicked or alt+f4 invoked).
     * 
     * @throws uninitialized_error if window is uninitialized.
     */
    bool window_should_close() const;

    /**
     * @brief Returns size of of the window in pixels.
     * 
     * @throws uninitialized_error if window is uninitialized.
     */
    glm::ivec2 get_window_size() const;

    /**
     * @brief Sets input type for cursor GLFW_CURSOR_DISABLED.
     *
     * @throws uninitialized_error if window is uninitialized.
     */
    void disable_cursor();

    /**
     * @brief Returns cursor position in window.
     * 
     * @throws @throws uninitialized_error if window is uninitialized.
     */
    glm::dvec2 get_cursor_position() const;

    /**
     * @brief Sets cursor position in window.
     * 
     * @throws uninitialized_error if window is uninitialized.
     */
    void set_cursor_position(const glm::dvec2& new_position);

    /**
     * @brief Checks if the key is pressed.
     * 
     * @param key_code for instance, GLFW_KEY_W.
     * @throws uninitialized_error if window is uninitialized.
     */
    bool is_key_pressed(int key_code) const;

    bool is_initialized() const noexcept {
        return static_cast<bool>(window);
    }

private:
    GLFWwindow* window = nullptr;
};
