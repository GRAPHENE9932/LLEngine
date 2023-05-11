#pragma once

#include <GL/glew.h>

class BRDFIntegrationMapperShader {
public:
    BRDFIntegrationMapperShader();

    void use_shader() const;

private:
    GLuint program_id = 0;
};
