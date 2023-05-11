#pragma once

#include "rendering/Texture.hpp"

#include <memory>

class EquirectangularMapperShader;
class IrradiancePrecomputerShader;
class SpecularPrefilterShader;
class BRDFIntegrationMapperShader;

class TextureLoadingError : std::runtime_error {
public:
    TextureLoadingError(const std::string& message) : std::runtime_error(message) {}
};

[[nodiscard]] std::unique_ptr<Texture>
panorama_to_cubemap(const Texture& panorama, EquirectangularMapperShader& shader);

[[nodiscard]] std::unique_ptr<Texture>
compute_irradiance_map(const Texture& environment_map, IrradiancePrecomputerShader& shader);

/**
 * @brief Computes prefiltered specular map with several mipmap levels for different roughness's.
 * 
 * @return Cubemap texture.
 */
[[nodiscard]] std::unique_ptr<Texture>
prefilter_specular_map(const Texture& environment_map, SpecularPrefilterShader& shader);

[[nodiscard]] std::unique_ptr<Texture>
compute_brdf_integration_map(BRDFIntegrationMapperShader& shader);

struct TexLoadingParams {
    GLenum magnification_filter = GL_LINEAR;
    GLenum minification_filter = GL_LINEAR;
    GLenum wrap_s = GL_CLAMP_TO_EDGE;
    GLenum wrap_t = GL_CLAMP_TO_EDGE;
    std::string file_path;
    std::streamsize offset = 0;
    std::streamsize size = 0; // Zero implies that loader must load to the end.
};

[[nodiscard]] std::unique_ptr<Texture> texture_from_ktx2(const TexLoadingParams& params);

[[nodiscard]] inline std::unique_ptr<Texture> texture_from_ktx2(const std::string& ktx_texture_path) {
    TexLoadingParams params;
    params.file_path = ktx_texture_path;
    return texture_from_ktx2(params);
}

[[nodiscard]] std::unique_ptr<Texture> texture_from_rgbe(const TexLoadingParams& params);

[[nodiscard]] inline std::unique_ptr<Texture> texture_from_rgbe(const std::string& rgbe_texture_path) {
    TexLoadingParams params;
    params.file_path = rgbe_texture_path;
    return texture_from_rgbe(params);
}