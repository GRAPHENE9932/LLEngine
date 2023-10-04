#include "rendering/GLFWWindow.hpp" // GLFWWindow
#include "nodes/Node.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h> // Various GLFW functions and constants.
#include <fmt/format.h>

#include <map>
#include <array>
#include <algorithm>
#include <functional>

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

    // Enable filtering of cubemap seams.
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

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
    glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void GLFWWindow::enable_cursor() {
    glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

glm::dvec2 GLFWWindow::get_cursor_position() const {
    glm::dvec2 result;
    glfwGetCursorPos(glfw_window, &result.x, &result.y);
    return result;
}

glm::u32vec2 GLFWWindow::get_window_size() const {
    glm::ivec2 result;
    glfwGetWindowSize(glfw_window, &result.x, &result.y);
    return result;
}

[[nodiscard]] glm::u32vec2 GLFWWindow::get_framebuffer_size() const {
    glm::ivec2 result;
    glfwGetFramebufferSize(glfw_window, &result.x, &result.y);
    return result;
}

void GLFWWindow::set_cursor_position(const glm::dvec2& new_position) {
    glfwSetCursorPos(glfw_window, new_position.x, new_position.y);
}

constexpr std::array<std::pair<Key, int>, 118> GLFW_KEY_CODE_TABLE {{
    {Key::SPACE, GLFW_KEY_SPACE},
    {Key::APOSTROPHE, GLFW_KEY_APOSTROPHE},
    {Key::COMMA, GLFW_KEY_COMMA},
    {Key::MINUS, GLFW_KEY_MINUS},
    {Key::DOT, GLFW_KEY_PERIOD},
    {Key::SLASH, GLFW_KEY_SLASH},
    {Key::ZERO, GLFW_KEY_0},
    {Key::ONE, GLFW_KEY_1},
    {Key::TWO, GLFW_KEY_2},
    {Key::THREE, GLFW_KEY_3},
    {Key::FOUR, GLFW_KEY_4},
    {Key::FIVE, GLFW_KEY_5},
    {Key::SIX, GLFW_KEY_6},
    {Key::SEVEN, GLFW_KEY_7},
    {Key::EIGHT, GLFW_KEY_8},
    {Key::NINE, GLFW_KEY_9},
    {Key::SEMICOLON, GLFW_KEY_SEMICOLON},
    {Key::EQUAL_SIGN, GLFW_KEY_EQUAL},
    {Key::A, GLFW_KEY_A},
    {Key::B, GLFW_KEY_B},
    {Key::C, GLFW_KEY_C},
    {Key::D, GLFW_KEY_D},
    {Key::E, GLFW_KEY_E},
    {Key::F, GLFW_KEY_F},
    {Key::G, GLFW_KEY_G},
    {Key::H, GLFW_KEY_H},
    {Key::I, GLFW_KEY_I},
    {Key::J, GLFW_KEY_J},
    {Key::K, GLFW_KEY_K},
    {Key::L, GLFW_KEY_L},
    {Key::M, GLFW_KEY_M},
    {Key::N, GLFW_KEY_N},
    {Key::O, GLFW_KEY_O},
    {Key::P, GLFW_KEY_P},
    {Key::Q, GLFW_KEY_Q},
    {Key::R, GLFW_KEY_R},
    {Key::S, GLFW_KEY_S},
    {Key::T, GLFW_KEY_T},
    {Key::U, GLFW_KEY_U},
    {Key::V, GLFW_KEY_V},
    {Key::W, GLFW_KEY_W},
    {Key::X, GLFW_KEY_X},
    {Key::Y, GLFW_KEY_Y},
    {Key::Z, GLFW_KEY_Z},
    {Key::LEFT_BRACKET, GLFW_KEY_LEFT_BRACKET},
    {Key::BACKSLASH, GLFW_KEY_APOSTROPHE},
    {Key::RIGHT_BRACKET, GLFW_KEY_RIGHT_BRACKET},
    {Key::GRAVE_ACCENT, GLFW_KEY_GRAVE_ACCENT},
    {Key::ESCAPE, GLFW_KEY_ESCAPE},
    {Key::ENTER, GLFW_KEY_ENTER},
    {Key::TAB, GLFW_KEY_TAB},
    {Key::BACKSPACE, GLFW_KEY_BACKSPACE},
    {Key::INSERT, GLFW_KEY_INSERT},
    {Key::DELETE, GLFW_KEY_DELETE},
    {Key::RIGHT, GLFW_KEY_RIGHT},
    {Key::LEFT, GLFW_KEY_LEFT},
    {Key::DOWN, GLFW_KEY_DOWN},
    {Key::UP, GLFW_KEY_UP},
    {Key::PAGE_UP, GLFW_KEY_PAGE_UP},
    {Key::PAGE_DOWN, GLFW_KEY_PAGE_DOWN},
    {Key::HOME, GLFW_KEY_HOME},
    {Key::END, GLFW_KEY_END},
    {Key::CAPS_LOCK, GLFW_KEY_CAPS_LOCK},
    {Key::SCROLL_LOCK, GLFW_KEY_SCROLL_LOCK},
    {Key::NUM_LOCK, GLFW_KEY_NUM_LOCK},
    {Key::PRINT_SCREEN, GLFW_KEY_PRINT_SCREEN},
    {Key::PAUSE, GLFW_KEY_PAUSE},
    {Key::F1, GLFW_KEY_F1},
    {Key::F2, GLFW_KEY_F2},
    {Key::F3, GLFW_KEY_F3},
    {Key::F4, GLFW_KEY_F4},
    {Key::F5, GLFW_KEY_F5},
    {Key::F6, GLFW_KEY_F6},
    {Key::F7, GLFW_KEY_F7},
    {Key::F8, GLFW_KEY_F8},
    {Key::F9, GLFW_KEY_F9},
    {Key::F10, GLFW_KEY_F10},
    {Key::F11, GLFW_KEY_F11},
    {Key::F12, GLFW_KEY_F12},
    {Key::F13, GLFW_KEY_F13},
    {Key::F14, GLFW_KEY_F14},
    {Key::F15, GLFW_KEY_F15},
    {Key::F16, GLFW_KEY_F16},
    {Key::F17, GLFW_KEY_F17},
    {Key::F18, GLFW_KEY_F18},
    {Key::F19, GLFW_KEY_F19},
    {Key::F20, GLFW_KEY_F20},
    {Key::F21, GLFW_KEY_F21},
    {Key::F22, GLFW_KEY_F22},
    {Key::F23, GLFW_KEY_F23},
    {Key::F24, GLFW_KEY_F24},
    {Key::F25, GLFW_KEY_F25},
    {Key::NUMPAD_ZERO, GLFW_KEY_KP_0},
    {Key::NUMPAD_ONE, GLFW_KEY_KP_1},
    {Key::NUMPAD_TWO, GLFW_KEY_KP_2},
    {Key::NUMPAD_THREE, GLFW_KEY_KP_3},
    {Key::NUMPAD_FOUR, GLFW_KEY_KP_4},
    {Key::NUMPAD_FIVE, GLFW_KEY_KP_5},
    {Key::NUMPAD_SIX, GLFW_KEY_KP_6},
    {Key::NUMPAD_SEVEN, GLFW_KEY_KP_7},
    {Key::NUMPAD_EIGHT, GLFW_KEY_KP_8},
    {Key::NUMPAD_NINE, GLFW_KEY_KP_9},
    {Key::NUMPAD_DOT, GLFW_KEY_KP_DECIMAL},
    {Key::NUMPAD_DIVIDE, GLFW_KEY_KP_DIVIDE},
    {Key::NUMPAD_MULTIPLY, GLFW_KEY_KP_MULTIPLY},
    {Key::NUMPAD_SUBTRACT, GLFW_KEY_KP_SUBTRACT},
    {Key::NUMPAD_ADD, GLFW_KEY_KP_ADD},
    {Key::NUMPAD_ENTER, GLFW_KEY_KP_ENTER},
    {Key::NUMPAD_EQUAL, GLFW_KEY_KP_EQUAL},
    {Key::LEFT_SHIFT, GLFW_KEY_LEFT_SHIFT},
    {Key::LEFT_CONTROL, GLFW_KEY_LEFT_CONTROL},
    {Key::LEFT_ALT, GLFW_KEY_LEFT_ALT},
    {Key::LEFT_SUPER, GLFW_KEY_LEFT_SUPER},
    {Key::RIGHT_SHIFT, GLFW_KEY_RIGHT},
    {Key::RIGHT_CONTROL, GLFW_KEY_RIGHT_SHIFT},
    {Key::RIGHT_ALT, GLFW_KEY_RIGHT_ALT},
    {Key::RIGHT_SUPER, GLFW_KEY_RIGHT_SUPER},
    {Key::MENU, GLFW_KEY_MENU}
}};

static int glfw_key_code_from_key(Key key) {
    const auto iter = std::find_if(
        GLFW_KEY_CODE_TABLE.begin(),
        GLFW_KEY_CODE_TABLE.end(),
        [&key] (const auto& pair) {
            return pair.first == key;
        }
    );

    if (iter == GLFW_KEY_CODE_TABLE.end()) {
        throw std::runtime_error("Invalid Key specified.");
    }

    return iter->second;
}

static Key key_from_glfw_key_code(int key_code) {
    const auto iter = std::find_if(
        GLFW_KEY_CODE_TABLE.begin(),
        GLFW_KEY_CODE_TABLE.end(),
        [&key_code] (const auto& pair) {
            return pair.second == key_code;
        }
    );

    if (iter == GLFW_KEY_CODE_TABLE.end()) {
        throw std::runtime_error("Unknown glfw key code.");
    }

    return iter->first;
}

bool GLFWWindow::is_key_pressed(Key key) const {
    return glfwGetKey(glfw_window, glfw_key_code_from_key(key)) == GLFW_PRESS;
}

[[nodiscard]] bool GLFWWindow::is_mouse_button_pressed(std::uint8_t button) const {
    return std::find(
        pressed_mouse_buttons.begin(),
        pressed_mouse_buttons.end(),
        button
    ) != pressed_mouse_buttons.end();
}

void GLFWWindow::subscribe_to_keyboard_key_event(Node& node) {
    if (keyboard_key_event_subscribers.size() == 0) {
        glfwSetKeyCallback(glfw_window, keyboard_key_callback);
    }

    keyboard_key_event_subscribers.push_back(node);
}

void GLFWWindow::unsubscribe_from_keyboard_key_event(Node& node) {
    const auto iter = std::find_if(
        keyboard_key_event_subscribers.begin(),
        keyboard_key_event_subscribers.end(),
        [&node] (std::reference_wrapper<Node> ref) {
            return std::addressof(ref.get()) == std::addressof(node);
        }
    );

    if (iter == keyboard_key_event_subscribers.end()) {
        throw std::runtime_error("Failed to unsubscribe from keyboard key event, as specified node was not found.");
    }

    keyboard_key_event_subscribers.erase(iter);

    if (keyboard_key_event_subscribers.empty()) {
        glfwSetKeyCallback(glfw_window, nullptr);
    }
}

void GLFWWindow::call_keyboard_key_event_subscribers(Key key, KeyEventType event_type) {
    for (Node& subscriber : keyboard_key_event_subscribers) {
        subscriber.key_event_callback(key, event_type);
    }
}

void GLFWWindow::keyboard_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    GLFWWindow* wrapper = window_to_wrapper_map[window];

    if (action == GLFW_PRESS) {
        wrapper->call_keyboard_key_event_subscribers(key_from_glfw_key_code(key), KeyEventType::PRESS_EVENT);
    }
    else if (action == GLFW_RELEASE) {
        wrapper->call_keyboard_key_event_subscribers(key_from_glfw_key_code(key), KeyEventType::RELEASE_EVENT);
    }
}