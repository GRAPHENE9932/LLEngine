#include <LLEngine/GameInstance.hpp>
#include <LLEngine/node_registration.hpp>

void register_nodes() {
    llengine::begin_nodes_registration();
    llengine::end_nodes_registration();
}

int main() {
    llengine::GameSettings settings;
    settings.json_scene_path = "res/maps/demo_map.json";
    settings.skybox_path = "res/textures/sky.hdr";
    settings.window_resolution = {1600, 1000};

    register_nodes();

    llengine::GameInstance instance(settings);
    instance.start();
}