#include <fstream>
#include <sstream>
#include <memory>

#include <GL/glew.h>

#include "shader_loader.hpp"

/// Loads and compiles vertex and fragment shaders from files,
/// compiles, links them and returns the program ID.
GLuint load_shaders(std::string vertex_shader_path, std::string fragment_shader_path) {
    ManagedProgramID program_id = glCreateProgram();

    ManagedShaderID vertex_shader_id = load_vertex_shader(vertex_shader_path);
    glAttachShader(program_id, vertex_shader_id);
    ManagedShaderID fragment_shader_id = load_fragment_shader(fragment_shader_path);
    glAttachShader(program_id, fragment_shader_id);

    glLinkProgram(program_id);

    GLint result;
    glGetProgramiv(program_id, GL_LINK_STATUS, &result);
    if (result == GL_FALSE) {
        GLint info_log_length;
        glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &info_log_length);

        std::unique_ptr<char[]> error_message = std::make_unique<char[]>(info_log_length + 1);
        glGetProgramInfoLog(program_id, info_log_length, nullptr, error_message.get());

        throw std::runtime_error(
            "Failed to link the program.\n"
            "Path to the vertex shader file: " + vertex_shader_path + "\n"
            "Path to the fragment shader file: " + fragment_shader_path + "\n"
            "Error message: " + error_message.get()
        );
    }

    glDetachShader(program_id, vertex_shader_id);
    glDetachShader(program_id, fragment_shader_id);

    return program_id.release();
}

/// Loads the vertex shader from file, compiles it and
/// returns the vertex shader ID.
ManagedShaderID load_vertex_shader(std::string& vertex_shader_path) {
    ManagedShaderID vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);

    std::ifstream file_stream;
    file_stream.open(vertex_shader_path, std::ios::in);
    if (!file_stream) {
        throw std::runtime_error(
            "Failed to read the vertex shader. Path to the file: " +
            vertex_shader_path
        );
    }

    std::stringstream shader_code_ss;
    shader_code_ss << file_stream.rdbuf();
    std::string shader_code = shader_code_ss.str();
    const char* shader_code_c = shader_code.c_str();
    glShaderSource(vertex_shader_id, 1, &shader_code_c, nullptr);
    glCompileShader(vertex_shader_id);

    GLint result;
    glGetShaderiv(vertex_shader_id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        GLint info_log_length;
        glGetShaderiv(vertex_shader_id, GL_INFO_LOG_LENGTH, &info_log_length);

        std::unique_ptr<char[]> error_message = std::make_unique<char[]>(info_log_length + 1);
        glGetShaderInfoLog(vertex_shader_id, info_log_length, nullptr, error_message.get());

        throw std::runtime_error(
            "Failed to compile the vertex shader.\n"
            "Path to the file: " + vertex_shader_path + "\n"
            "Error message: " + error_message.get()
        );
    }

    return vertex_shader_id.release();
}

/// Loads the fragment shader from file, compiles it and
/// returns the fragment shader shader ID.
ManagedShaderID load_fragment_shader(std::string& fragment_shader_path) {
    ManagedShaderID fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

    std::ifstream file_stream;
    file_stream.open(fragment_shader_path, std::ios::in);
    if (!file_stream) {
        throw std::runtime_error(
            "Failed to read the fragment shader. Path to the file: " +
            fragment_shader_path
        );
    }

    std::stringstream shader_code_ss;
    shader_code_ss << file_stream.rdbuf();
    std::string shader_code = shader_code_ss.str();
    const char* shader_code_c = shader_code.c_str();
    glShaderSource(fragment_shader_id, 1, &shader_code_c, nullptr);
    glCompileShader(fragment_shader_id);

    GLint result;
    glGetShaderiv(fragment_shader_id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        GLint info_log_length;
        glGetShaderiv(fragment_shader_id, GL_INFO_LOG_LENGTH, &info_log_length);

        std::unique_ptr<char[]> error_message = std::make_unique<char[]>(info_log_length + 1);
        glGetShaderInfoLog(fragment_shader_id, info_log_length, nullptr, error_message.get());

        throw std::runtime_error(
            "Failed to compile the fragment shader.\n"
            "Path to the file: " + fragment_shader_path + "\n"
            "Error message: " + error_message.get()
        );
    }

    return fragment_shader_id.release();
}
