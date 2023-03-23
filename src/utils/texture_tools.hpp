#pragma once

#include "common/core/Texture.hpp"

#include <memory>

class EquirectangularMapperShader;
class IrradiancePrecomputerShader;
class SpecularPrefilterShader;
class BRDFIntegrationMapperShader;

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