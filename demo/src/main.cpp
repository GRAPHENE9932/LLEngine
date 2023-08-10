#include "FPSTextNode.hpp"
#include "MainCamera.hpp"

#include <LLEngine/GameInstance.hpp>
#include <LLEngine/node_registration.hpp>

void register_nodes() {
    llengine::begin_nodes_registration();
    llengine::register_node_type<FPSTextNode, llengine::TextNode>("fps_text_node");
    llengine::register_node_type<MainCamera, llengine::SpectatorCameraNode>("main_camera");
    llengine::end_nodes_registration();
}

int main() {
    llengine::GameSettings settings;
    settings.json_scene_path = "res/maps/demo_map.json";
    settings.skybox_path = "res/textures/sky.hdr";
    settings.window_resolution = {1600, 1000};
    settings.quality_settings.shadow_mapping_enabled = true;
    settings.quality_settings.shadow_map_size = {2048, 2048};

    register_nodes();

    llengine::GameInstance instance(settings);
    instance.start();
}