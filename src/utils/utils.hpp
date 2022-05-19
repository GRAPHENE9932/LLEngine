#pragma once

#include <string>
#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <GL/glew.h>

namespace utils {
    struct ManagedShaderID {
    public:
        GLuint shader_id;

        ManagedShaderID(GLuint orig_shader_id) noexcept : shader_id(orig_shader_id) {

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
        GLuint program_id;

        ManagedProgramID(GLuint orig_program_id) noexcept : program_id(orig_program_id) {

        }

        ~ManagedProgramID() {
            if (!released)
                glDeleteProgram(program_id);
        }

        operator GLuint() const noexcept {
            return program_id;
        }

        GLuint release() noexcept {
            released = true;
            return program_id;
        }

    private:
        bool released = false;
    };

    GLuint load_shaders(std::string vertex_shader_path, std::string fragment_shader_path);
    ManagedShaderID load_vertex_shader(std::string& vertex_shader_path);
    ManagedShaderID load_fragment_shader(std::string& fragment_shader_path);

    /// Load the wavefront OBJ file (limited).
    /// Limitations:
    ///  - List of vertices, UVs and normals must be before the list of faces.
    ///  - All faces must have vertex, UV and normal index at the same time.
    ///  - Materials are ignored.
    ///  - Parameter space vertices are ignored.
    ///  - Line elements are ignored.
    ///  - W coordinates in both vertices list and UVs list are ignored.
    ///  - Objects and groups are ignored.
    ///  - Smooth shading option is ignored.
    void load_wavefront_obj(std::string file_path, std::vector<glm::vec3>& vertices_out,
                            std::vector<glm::vec2>& uvs_out, std::vector<glm::vec3>& normals_out);
}
