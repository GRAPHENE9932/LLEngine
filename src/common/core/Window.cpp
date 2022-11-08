#include <sstream> // std::stringstream

#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include "Window.hpp"
#include "common/core/uninitialized_error.hpp"

size_t Window::windows_count = 0;

void handle_glfw_error() {
    const char* error_desc;
    const int error_code = glfwGetError(&error_desc);
    std::stringstream ss;
    ss << "Failed to create the window (code 0x" << std::hex <<
            error_code << "): " << error_desc;

    throw std::runtime_error(ss.str());
}

void initialize_glfw() {
    if (glfwInit() != GLFW_TRUE)
        handle_glfw_error();
}

void initialize_glew() {
    int glew_init_res = glewInit();
    if (glew_init_res != GLEW_OK) {
        throw std::runtime_error(
            std::string("Failed to initialize GLEW. Error code: " + std::to_string(glew_init_res))
        );
    }
}

void initialize_opengl() {
    // Create the VAO.
    GLuint vertex_array_id;
    glGenVertexArrays(1, &vertex_array_id);
    glBindVertexArray(vertex_array_id);

    // Enable depth test.
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    // Enable gamma correction.
    glEnable(GL_FRAMEBUFFER_SRGB);

    // Set blend function.
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Enable backface culling.
    glEnable(GL_CULL_FACE);
}

void terminate_glfw() {
    glfwTerminate();
}

void Window::initialize(glm::ivec2 extents, const char* title,
                        int gl_version_major, int gl_version_minor) {
    if (windows_count == 0)
        initialize_glfw();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, gl_version_major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, gl_version_minor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(extents.x, extents.y, title, nullptr, nullptr);
    if (!window)
        handle_glfw_error();

    glfwMakeContextCurrent(window);

    if (windows_count == 0) {
        initialize_glew();
        initialize_opengl();
    }

    windows_count++;
}

Window::~Window() {
    if (window) {
        glfwDestroyWindow(window);

        windows_count--;
        if (windows_count == 0)
            terminate_glfw();
    }
}

void Window::glfw_swap_buffers() {
    if (!is_initialized()) {
        throw uninitialized_error("Failed to swap buffers.", "Window");
    }

    glfwSwapBuffers(window);
}

bool Window::window_should_close() const {
    if (!is_initialized()) {
        throw uninitialized_error("Failed to check if window should close.",
            "Window");
    }

    return glfwWindowShouldClose(window);
}

glm::ivec2 Window::get_window_size() const {
    if (!is_initialized()) {
        throw uninitialized_error("Failed to get window size.",
            "Window");
    }

    glm::ivec2 result;
    glfwGetWindowSize(window, &result.x, &result.y);
    return result;
}

void Window::disable_cursor() {
    if (!is_initialized()) {
        throw uninitialized_error("Failed to disable cursor.",
            "Window");
    }

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

glm::dvec2 Window::get_cursor_position() const {
    if (!is_initialized()) {
        throw uninitialized_error("Failed to retrieve cursor position.",
            "Window");
    }

    glm::dvec2 result;
    glfwGetCursorPos(window, &result.x, &result.y);
    return result;
}

void Window::set_cursor_position(const glm::dvec2& new_position) {
    if (!is_initialized()) {
        throw uninitialized_error("Failed to set cursor position.",
            "Window");
    }

    glfwSetCursorPos(window, new_position.x, new_position.y);
}

bool Window::is_key_pressed(int key_code) const {
    if (!is_initialized()) {
        throw uninitialized_error("Failed to check keyboard key.",
            "Window");
    }

    return glfwGetKey(window, key_code) == GLFW_PRESS;
}
