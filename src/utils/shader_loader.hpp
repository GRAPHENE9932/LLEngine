#pragma once

#include <string>
#include <vector>

#include <GL/glew.h>

#include "datatypes.hpp"

namespace llengine {
struct ManagedShaderID {
public:
    ShaderID shader_id;

    ManagedShaderID(ShaderID orig_shader_id) noexcept : shader_id(orig_shader_id) {

    }

    ~ManagedShaderID() {
        if (!released)
            glDeleteShader(shader_id);
    }

    operator GLuint() const noexcept {
        return shader_id;
    }

    GLuint release() noexcept {
        released = true;
        return shader_id;
    }

private:
    bool released = false;
};

struct ManagedProgramID {
public:
    ShaderID program_id;

    ManagedProgramID(ShaderID orig_program_id) noexcept : program_id(orig_program_id) {

    }

    ~ManagedProgramID() {
        if (!released)
            glDeleteProgram(program_id);
    }

    operator ShaderID() const noexcept {
        return program_id;
    }

    ShaderID release() noexcept {
        released = true;
        return program_id;
    }

private:
    bool released = false;
};

ShaderID load_shaders_from_files(
    std::string_view vertex_shader_path,
    std::string_view fragment_shader_path,
    const std::vector<std::string>& defines = {}
);

ShaderID load_shaders(
    std::string_view vertex_shader_code,
    std::string_view fragment_shader_code,
    const std::vector<std::string>& defines = {}
);

ManagedShaderID load_vertex_shader(std::string_view vertex_shader_code,
                                   const std::vector<std::string>& defines);
ManagedShaderID load_fragment_shader(std::string_view fragment_shader_code,
                                     const std::vector<std::string>& defines);
}