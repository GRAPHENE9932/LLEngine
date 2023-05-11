#include "BRDFIntegrationMapperShader.hpp"
#include "utils/shader_loader.hpp"

BRDFIntegrationMapperShader::BRDFIntegrationMapperShader() {
    program_id = load_shaders(
        "res/shaders/brdf_integration_mapper.vert",
        "res/shaders/brdf_integration_mapper.frag"
    );
}

void BRDFIntegrationMapperShader::use_shader() const {
    glUseProgram(program_id);
}