#include "SceneFile.hpp" // SceneFile
#include "GLTF.hpp" // GLTF
#include "common/SceneJSON.hpp"

#include <stdexcept> // std::runtime_error

std::unique_ptr<SceneFile> SceneFile::load_from_file(const std::filesystem::path& file_path) {
    const auto extension = file_path.extension().string();

    if (extension == ".glb" || extension == ".gltf") {
        return std::make_unique<GLTF>(file_path.string());
    }
    else if (extension == ".json") {
        return std::make_unique<SceneJSON>(file_path.string());
    }
    else {
        throw std::runtime_error("Non-supported or unrecognized scene file provided.");
    }
}
