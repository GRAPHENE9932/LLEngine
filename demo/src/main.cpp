#include "FPSTextNode.hpp"
#include "MainCamera.hpp"
#include "BulletNode.hpp"
#include "ExposureTextNode.hpp"
#include "ResolutionTextNode.hpp"
#include "FadingPointLightNode.hpp"

#include <LLEngine/GameInstance.hpp>
#include <LLEngine/node_registration.hpp>

void register_nodes() {
    llengine::begin_nodes_registration();
    llengine::register_node_type<FPSTextNode, llengine::TextNode>("fps_text_node");
    llengine::register_node_type<MainCamera, llengine::SpectatorCameraNode>("main_camera");
    llengine::register_node_type<BulletNode, llengine::BulletRigidBodyNode>("bullet_node");
    llengine::register_node_type<ExposureTextNode, llengine::TextNode>("exposure_text_node");
    llengine::register_node_type<ResolutionTextNode, llengine::TextNode>("resolution_text_node");
    llengine::register_node_type<FadingPointLightNode, llengine::PointLightNode>("fading_point_light_node");
    llengine::end_nodes_registration();
}

int main() {
    llengine::GameSettings settings;
    settings.window_title = "LLEngine demo";
    settings.json_scene_path = "res/maps/demo_map.json";
    settings.skybox_path = "res/textures/sky.hdr";
    settings.window_resolution = {1600, 1000};
    settings.quality_settings.shadow_mapping_enabled = true;
    settings.quality_settings.shadow_map_size = {2048, 2048};
    settings.quality_settings.anisotropy = 16.0f;

    register_nodes();

    llengine::GameInstance instance(settings);
    instance.start();
}