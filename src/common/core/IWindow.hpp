#pragma once

#include <glm/vec2.hpp> // glm::dvec2, glm::ivec2

enum class Key : unsigned short {
    SPACE = ' ',
    APOSTROPHE = '\'',
    COMMA = ',',
    MINUS = '-',
    DOT = '.',
    SLASH = '/',
    ZERO = '0',
    ONE = '1', TWO = '2', THREE = '3',
    FOUR = '4', FIVE = '5', SIX = '6',
    SEVEN = '7', EIGHT = '8', NINE = '9',
    SEMICOLON = ';',
    EQUAL_SIGN = '=',
    A = 'a', B = 'b', C = 'c', D = 'd',
    E = 'e', F = 'f', G = 'g', H = 'h',
    I = 'i', J = 'j', K = 'k', L = 'l',
    M = 'm', N = 'n', O = 'o', P = 'p',
    Q = 'q', R = 'r', S = 's', T = 't',
    U = 'u', V = 'v', W = 'w', X = 'x',
    Y = 'y', Z = 'z',
    LEFT_BRACKET = '[',
    BACKSLASH = '\\',
    RIGHT_BRACKET = ']',
    GRAVE_ACCENT = '`',
    ESCAPE = 256,
    ENTER = 257,
    TAB = 258,
    BACKSPACE = 259,
    INSERT = 260,
    DELETE = 261,
    RIGHT = 262,
    LEFT = 263,
    DOWN = 264,
    UP = 265,
    PAGE_UP = 266,
    PAGE_DOWN = 267,
    HOME = 268,
    END = 269,
    CAPS_LOCK = 280,
    SCROLL_LOCK = 281,
    NUM_LOCK = 282,
    PRINT_SCREEN = 283,
    PAUSE = 284,
    F1 = 290, F2 = 291, F3 = 292, F4 = 293,
    F5 = 294, F6 = 295, F7 = 296, F8 = 297,
    F9 = 298, F10 = 299, F11 = 300, F12 = 301,
    F13 = 302, F14 = 303, F15 = 304, F16 = 305,
    F17 = 306, F18 = 307, F19 = 308, F20 = 309,
    F21 = 310, F22 = 311, F23 = 312, F24 = 313,
    F25 = 314,
    NUMPAD_ZERO = 320, NUMPAD_ONE = 321,
    NUMPAD_TWO = 322, NUMPAD_THREE = 323,
    NUMPAD_FOUR = 324, NUMPAD_FIVE = 325,
    NUMPAD_SIX = 326, NUMPAD_SEVEN = 327,
    NUMPAD_EIGHT = 328, NUMPAD_NINE = 329,
    NUMPAD_DOT = 330, NUMPAD_DIVIDE = 331,
    NUMPAD_MULTIPLY = 332, NUMPAD_SUBSTRACT = 333,
    NUMPAD_ADD = 334, NUMPAD_ENTER = 335,
    NUMPAD_EQUAL = 336,
    LEFT_SHIFT = 340,
    LEFT_CONTROL = 341,
    LEFT_ALT = 342,
    LEFT_SUPER = 343,
    RIGHT_SHIFT = 344,
    RIGHT_CONTROL = 345,
    RIGHT_ALT = 346,
    RIGHT_SUPER = 347,
    MENU = 348
};

class IWindow {
public:
    virtual void swap_buffers() = 0;
    /**
     * @brief Checks if the window should close (for example,
     * when the close button is clicked or alt+f4 invoked).
     *
     * @throws uninitialized_error if window is uninitialized.
     */
    virtual bool window_should_close() const = 0;
    virtual void disable_cursor() = 0;
    virtual glm::dvec2 get_cursor_position() const = 0;
    virtual glm::ivec2 get_window_size() const = 0;
    virtual void set_cursor_position(const glm::dvec2& new_position) = 0;
    /**
     * @returns if specified keyboard key is pressed. If
     * the specified key is invalid, always returns false.
     */
    virtual bool is_key_pressed(Key key) const = 0;
};
