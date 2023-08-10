#pragma once

#include "datatypes.hpp"

namespace llengine {
class BRDFIntegrationMapperShader {
public:
    BRDFIntegrationMapperShader();

    void use_shader() const;

private:
    ShaderID program_id = 0;
};
}