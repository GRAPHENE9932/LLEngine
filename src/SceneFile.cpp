#include "SceneFile.hpp" // SceneFile
#include "GLTF.hpp" // GLTF
#include "SceneJSON.hpp"

#include <stdexcept> // std::runtime_error

using namespace llengine;

std::unique_ptr<SceneFile> SceneFile::load_from_file(std::string_view file_path) {
    if (file_path.ends_with(".glb") || file_path.ends_with(".gltf")) {
        return std::make_unique<GLTF>(file_path);
    }
    else if (file_path.ends_with(".json")) {
        return std::make_unique<SceneJSON>(file_path);
    }
    else {
        throw std::runtime_error("Non-supported or unrecognized scene file provided.");
    }
}
