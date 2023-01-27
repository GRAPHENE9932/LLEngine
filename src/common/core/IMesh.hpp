#pragma once

#include <GL/glew.h> // GLuint, GLenum

class IMesh {
public:
    virtual GLuint get_indices_id() = 0;
    virtual GLuint get_vertices_id() = 0;
    virtual GLuint get_uvs_id() = 0;
    virtual GLuint get_normals_id() = 0;
    virtual GLuint get_tangents_id() = 0;

    virtual size_t get_amount_of_vertices() = 0;
    virtual GLenum get_indices_type() = 0;

    virtual bool is_indexed() = 0;

    virtual ~IMesh() = default;
};
