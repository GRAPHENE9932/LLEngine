#pragma once

#include "common/core/Texture.hpp"

#include <memory>

std::shared_ptr<Texture> panorama_to_cubemap(const Texture& panorama);