#pragma once

#include "common/core/Texture.hpp"
#include "common/core/shaders/EquirectangularMapperShader.hpp"

#include <memory>

std::shared_ptr<Texture> panorama_to_cubemap(const Texture& panorama, EquirectangularMapperShader& shader);