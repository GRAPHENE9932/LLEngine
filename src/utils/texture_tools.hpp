#pragma once

#include "common/core/Texture.hpp"

#include <memory>

class EquirectangularMapperShader;
class IrradiancePrecomputerShader;

std::unique_ptr<Texture> panorama_to_cubemap(const Texture& panorama, EquirectangularMapperShader& shader);
std::unique_ptr<Texture> compute_irradiance_map(const Texture& environment_map, IrradiancePrecomputerShader& shader);
