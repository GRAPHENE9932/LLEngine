#include "BRDFIntegrationMapperShader.hpp"
#include "utils/shader_loader.hpp"

using namespace llengine;

constexpr std::string_view VERTEX_SHADER_TEXT = 
    #include "shaders/objects/pbr/brdf_integration_mapper.vert"
;
constexpr std::string_view FRAGMENT_SHADER_TEXT =
    #include "shaders/objects/pbr/brdf_integration_mapper.frag"
;

BRDFIntegrationMapperShader::BRDFIntegrationMapperShader() {
    program_id = load_shaders(
        VERTEX_SHADER_TEXT,
        FRAGMENT_SHADER_TEXT
    );
}

void BRDFIntegrationMapperShader::use_shader() const {
    glUseProgram(program_id);
}