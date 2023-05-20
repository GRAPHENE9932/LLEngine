#include "rendering/GLFWWindow.hpp" // GLFWWindow

#include <GL/glew.h> // glewInit, GLEW_OK
#include <GLFW/glfw3.h> // Various GLFW functions and constants.
#include <fmt/format.h>

#include <map>
#include <algorithm>

using namespace llengine;

std::map<GLFWwindow*, GLFWWindow*> window_to_wrapper_map;

size_t GLFWWindow::glfw_windows_count = 0;

static void handle_glfw_error() {
    const char* error_desc;
    const int error_code = glfwGetError(&error_desc);
    throw std::runtime_error(fmt::format(
        "Failed to create the window (code {:x}): {}", error_code, error_desc
    ));
}

static void initialize_glfw() {
    if (glfwInit() != GLFW_TRUE)
        handle_glfw_error();
}

static void initialize_glew() {
    GLenum glew_init_res = glewInit();
    if (glew_init_res != GLEW_OK) {
        throw std::runtime_error(
            fmt::format(
                "Failed to initialize GLEW. Error code: {}. String: {}",
                glew_init_res, reinterpret_cast<const char*>(glewGetErrorString(glew_init_res))
            )
        );
    }
}

static void initialize_opengl() {
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

GLFWWindow::GLFWWindow(glm::ivec2 extents, std::string_view title,
                       int gl_version_major, int gl_version_minor) {
    if (glfw_windows_count == 0)
        initialize_glfw();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, gl_version_major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, gl_version_minor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfw_window = glfwCreateWindow(extents.x, extents.y, std::string(title).c_str(), nullptr, nullptr);
    if (!glfw_window)
        handle_glfw_error();

    glfwMakeContextCurrent(glfw_window);
    glfwSwapInterval(0);

    if (glfw_windows_count == 0) {
        initialize_glew();
        initialize_opengl();
    }

    glfw_windows_count++;
    window_to_wrapper_map[glfw_window] = this;
    glfwSetMouseButtonCallback(glfw_window, mouse_button_callback);
}

void GLFWWindow::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    GLFWWindow* wrapper = window_to_wrapper_map[window];

    if (action == GLFW_PRESS) {
        wrapper->pressed_mouse_buttons.push_back(button);
    }
    else if (action == GLFW_RELEASE) {
        wrapper->pressed_mouse_buttons.erase(std::find(
            wrapper->pressed_mouse_buttons.begin(),
            wrapper->pressed_mouse_buttons.end(),
            static_cast<std::uint8_t>(button)
        ));
    }
}

GLFWWindow::~GLFWWindow() {
    if (glfw_window) {
        glfwDestroyWindow(glfw_window);

        glfw_windows_count--;
        window_to_wrapper_map.erase(glfw_window);
        if (glfw_windows_count == 0)
            terminate_glfw();
    }
}

void GLFWWindow::swap_buffers() {
    glfwSwapBuffers(glfw_window);
}

bool GLFWWindow::window_should_close() const {
    return glfwWindowShouldClose(glfw_window);
}

void GLFWWindow::disable_cursor() {
    //glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

glm::dvec2 GLFWWindow::get_cursor_position() const {
    glm::dvec2 result;
    glfwGetCursorPos(glfw_window, &result.x, &result.y);
    return result;
}

glm::ivec2 GLFWWindow::get_window_size() const {
    glm::ivec2 result;
    glfwGetWindowSize(glfw_window, &result.x, &result.y);
    return result;
}

void GLFWWindow::set_cursor_position(const glm::dvec2& new_position) {
    //glfwSetCursorPos(glfw_window, new_position.x, new_position.y);
}

static int get_glfw_key_code(Key key) {
    switch (key) {
    case Key::SPACE: return GLFW_KEY_SPACE;
    case Key::APOSTROPHE: return GLFW_KEY_APOSTROPHE;
    case Key::COMMA: return GLFW_KEY_COMMA;
    case Key::MINUS: return GLFW_KEY_MINUS;
    case Key::DOT: return GLFW_KEY_PERIOD;
    case Key::SLASH: return GLFW_KEY_SLASH;
    case Key::ZERO: return GLFW_KEY_0;
    case Key::ONE: return GLFW_KEY_1;
    case Key::TWO: return GLFW_KEY_2;
    case Key::THREE: return GLFW_KEY_3;
    case Key::FOUR: return GLFW_KEY_4;
    case Key::FIVE: return GLFW_KEY_5;
    case Key::SIX: return GLFW_KEY_6;
    case Key::SEVEN: return GLFW_KEY_7;
    case Key::EIGHT: return GLFW_KEY_8;
    case Key::NINE: return GLFW_KEY_9;
    case Key::SEMICOLON: return GLFW_KEY_SEMICOLON;
    case Key::EQUAL_SIGN: return GLFW_KEY_EQUAL;
    case Key::A: return GLFW_KEY_A;
    case Key::B: return GLFW_KEY_B;
    case Key::C: return GLFW_KEY_C;
    case Key::D: return GLFW_KEY_D;
    case Key::E: return GLFW_KEY_E;
    case Key::F: return GLFW_KEY_F;
    case Key::G: return GLFW_KEY_G;
    case Key::H: return GLFW_KEY_H;
    case Key::I: return GLFW_KEY_I;
    case Key::J: return GLFW_KEY_J;
    case Key::K: return GLFW_KEY_K;
    case Key::L: return GLFW_KEY_L;
    case Key::M: return GLFW_KEY_M;
    case Key::N: return GLFW_KEY_N;
    case Key::O: return GLFW_KEY_O;
    case Key::P: return GLFW_KEY_P;
    case Key::Q: return GLFW_KEY_Q;
    case Key::R: return GLFW_KEY_R;
    case Key::S: return GLFW_KEY_S;
    case Key::T: return GLFW_KEY_T;
    case Key::U: return GLFW_KEY_U;
    case Key::V: return GLFW_KEY_V;
    case Key::W: return GLFW_KEY_W;
    case Key::X: return GLFW_KEY_X;
    case Key::Y: return GLFW_KEY_Y;
    case Key::Z: return GLFW_KEY_Z;
    case Key::LEFT_BRACKET: return GLFW_KEY_LEFT_BRACKET;
    case Key::BACKSLASH: return GLFW_KEY_APOSTROPHE;
    case Key::RIGHT_BRACKET: return GLFW_KEY_RIGHT_BRACKET;
    case Key::GRAVE_ACCENT: return GLFW_KEY_GRAVE_ACCENT;
    case Key::ESCAPE: return GLFW_KEY_ESCAPE;
    case Key::ENTER: return GLFW_KEY_ENTER;
    case Key::TAB: return GLFW_KEY_TAB;
    case Key::BACKSPACE: return GLFW_KEY_BACKSPACE;
    case Key::INSERT: return GLFW_KEY_INSERT;
    case Key::DELETE: return GLFW_KEY_DELETE;
    case Key::RIGHT: return GLFW_KEY_RIGHT;
    case Key::LEFT: return GLFW_KEY_LEFT;
    case Key::DOWN: return GLFW_KEY_DOWN;
    case Key::UP: return GLFW_KEY_UP;
    case Key::PAGE_UP: return GLFW_KEY_PAGE_UP;
    case Key::PAGE_DOWN: return GLFW_KEY_PAGE_DOWN;
    case Key::HOME: return GLFW_KEY_HOME;
    case Key::END: return GLFW_KEY_END;
    case Key::CAPS_LOCK: return GLFW_KEY_CAPS_LOCK;
    case Key::SCROLL_LOCK: return GLFW_KEY_SCROLL_LOCK;
    case Key::NUM_LOCK: return GLFW_KEY_NUM_LOCK;
    case Key::PRINT_SCREEN: return GLFW_KEY_PRINT_SCREEN;
    case Key::PAUSE: return GLFW_KEY_PAUSE;
    case Key::F1: return GLFW_KEY_F1;
    case Key::F2: return GLFW_KEY_F2;
    case Key::F3: return GLFW_KEY_F3;
    case Key::F4: return GLFW_KEY_F4;
    case Key::F5: return GLFW_KEY_F5;
    case Key::F6: return GLFW_KEY_F6;
    case Key::F7: return GLFW_KEY_F7;
    case Key::F8: return GLFW_KEY_F8;
    case Key::F9: return GLFW_KEY_F9;
    case Key::F10: return GLFW_KEY_F10;
    case Key::F11: return GLFW_KEY_F11;
    case Key::F12: return GLFW_KEY_F12;
    case Key::F13: return GLFW_KEY_F13;
    case Key::F14: return GLFW_KEY_F14;
    case Key::F15: return GLFW_KEY_F15;
    case Key::F16: return GLFW_KEY_F16;
    case Key::F17: return GLFW_KEY_F17;
    case Key::F18: return GLFW_KEY_F18;
    case Key::F19: return GLFW_KEY_F19;
    case Key::F20: return GLFW_KEY_F20;
    case Key::F21: return GLFW_KEY_F21;
    case Key::F22: return GLFW_KEY_F22;
    case Key::F23: return GLFW_KEY_F23;
    case Key::F24: return GLFW_KEY_F24;
    case Key::F25: return GLFW_KEY_F25;
    case Key::NUMPAD_ZERO: return GLFW_KEY_KP_0;
    case Key::NUMPAD_ONE: return GLFW_KEY_KP_1;
    case Key::NUMPAD_TWO: return GLFW_KEY_KP_2;
    case Key::NUMPAD_THREE: return GLFW_KEY_KP_3;
    case Key::NUMPAD_FOUR: return GLFW_KEY_KP_4;
    case Key::NUMPAD_FIVE: return GLFW_KEY_KP_5;
    case Key::NUMPAD_SIX: return GLFW_KEY_KP_6;
    case Key::NUMPAD_SEVEN: return GLFW_KEY_KP_7;
    case Key::NUMPAD_EIGHT: return GLFW_KEY_KP_8;
    case Key::NUMPAD_NINE: return GLFW_KEY_KP_9;
    case Key::NUMPAD_DOT: return GLFW_KEY_KP_DECIMAL;
    case Key::NUMPAD_DIVIDE: return GLFW_KEY_KP_DIVIDE;
    case Key::NUMPAD_MULTIPLY: return GLFW_KEY_KP_MULTIPLY;
    case Key::NUMPAD_SUBTRACT: return GLFW_KEY_KP_SUBTRACT;
    case Key::NUMPAD_ADD: return GLFW_KEY_KP_ADD;
    case Key::NUMPAD_ENTER: return GLFW_KEY_KP_ENTER;
    case Key::NUMPAD_EQUAL: return GLFW_KEY_KP_EQUAL;
    case Key::LEFT_SHIFT: return GLFW_KEY_LEFT_SHIFT;
    case Key::LEFT_CONTROL: return GLFW_KEY_LEFT_CONTROL;
    case Key::LEFT_ALT: return GLFW_KEY_LEFT_ALT;
    case Key::LEFT_SUPER: return GLFW_KEY_LEFT_SUPER;
    case Key::RIGHT_SHIFT: return GLFW_KEY_RIGHT;
    case Key::RIGHT_CONTROL: return GLFW_KEY_RIGHT_SHIFT;
    case Key::RIGHT_ALT: return GLFW_KEY_RIGHT_ALT;
    case Key::RIGHT_SUPER: return GLFW_KEY_RIGHT_SUPER;
    case Key::MENU: return GLFW_KEY_MENU;
    default: return GLFW_KEY_UNKNOWN;
    }
}

bool GLFWWindow::is_key_pressed(Key key) const {
    return glfwGetKey(glfw_window, get_glfw_key_code(key)) == GLFW_PRESS;
}

[[nodiscard]] bool GLFWWindow::is_mouse_button_pressed(std::uint8_t button) const {
    return std::find(
        pressed_mouse_buttons.begin(),
        pressed_mouse_buttons.end(),
        button
    ) != pressed_mouse_buttons.end();
}