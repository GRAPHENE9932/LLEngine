#pragma once

#include <GL/glew.h>

namespace llengine {
class BRDFIntegrationMapperShader {
public:
    BRDFIntegrationMapperShader();

    void use_shader() const;

private:
    GLuint program_id = 0;
};
}