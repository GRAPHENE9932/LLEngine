#include "OpenGLDebug.hpp"
#include "logger.hpp"
#include <GL/glew.h>
#include <fmt/format.h>
#include <string_view>

namespace llengine {
static std::string_view get_debug_type_by_enum(GLenum type) {
    switch (type) {
        case GL_DEBUG_TYPE_ERROR:
            return "ERROR";
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            return "DEPRECATED_BEHAVIOR";
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            return "UNDEFINED_BEHAVIOR";
        case GL_DEBUG_TYPE_PORTABILITY:
            return "PORTABILITY";
        case GL_DEBUG_TYPE_PERFORMANCE:
            return "PERFORMANCE";
        case GL_DEBUG_TYPE_MARKER:
            return "MARKER";
        case GL_DEBUG_TYPE_PUSH_GROUP:
            return "PUSH_GROUP";
        case GL_DEBUG_TYPE_POP_GROUP:
            return "POP_GROUP";
        case GL_DEBUG_TYPE_OTHER:
            return "OTHER";
        default:
            return "UNKNOWN";
    }
}

static logger::Severity get_logger_severity_by_enum(GLenum severity) {
    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH:
            return logger::Severity::ERROR;
        case GL_DEBUG_SEVERITY_MEDIUM:
            return logger::Severity::WARNING;
        case GL_DEBUG_SEVERITY_LOW:
        case GL_DEBUG_SEVERITY_NOTIFICATION:
        default:
            return logger::Severity::INFO;
    }
}

static std::string_view get_severity_by_enum(GLenum severity) {
    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH:
            return "HIGH";
        case GL_DEBUG_SEVERITY_MEDIUM:
            return "MEDIUM";
        case GL_DEBUG_SEVERITY_LOW:
            return "LOW";
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            return "NOTIFICATION";
        default:
            return "UNKNOWN";
    }
}

static std::string_view get_source_by_enum(GLenum source) {
    switch (source) {
        case GL_DEBUG_SOURCE_API:
            return "API";
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            return "WINDOW_SYSTEM";
        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            return "SHADER_COMPILER";
        case GL_DEBUG_SOURCE_THIRD_PARTY:
            return "THIRD_PARTY";
        case GL_DEBUG_SOURCE_APPLICATION:
            return "APPLICATION";
        case GL_DEBUG_SOURCE_OTHER:
            return "OTHER";
        default:
            return "UNKNOWN";
    }
}

void opengl_debug_message_callback(
    GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* user_param
) {
    if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) {
        return;
    }
    
    std::string_view source_str = get_source_by_enum(source);
    std::string_view type_str = get_debug_type_by_enum(type);
    std::string_view severity_str = get_severity_by_enum(severity);
    std::string_view message_str(message, length);

    logger::log(
        get_logger_severity_by_enum(severity),
        fmt::format(
            "OpenGL debug message: source: \"{}\", type: \"{}\", severity: \"{}\", message: \"{}\"",
            source_str,
            type_str,
            severity_str,
            message_str
        )
    );
}

void enable_opengl_debug_logs() {
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(opengl_debug_message_callback, nullptr);
}
}