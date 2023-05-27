#include "LLEngine/GameInstance.hpp"

int main() {
    llengine::GameSettings settings;
    settings.json_scene_path = "res/maps/demo_map.json";
    settings.skybox_path = "res/textures/sky.hdr";
    settings.window_resolution = {1600, 1000};

    llengine::GameInstance instance(settings);
    instance.start();
}