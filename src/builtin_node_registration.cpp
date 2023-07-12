#include "node_registration.hpp"
#include "nodes/SpatialNode.hpp"
#include "nodes/gui/ButtonNode.hpp"
#include "nodes/gui/GUINode.hpp"
//#include "nodes/gui/TextNode.hpp"
#include "nodes/gui/GUICanvas.hpp"
#include "nodes/rendering/SpectatorCameraNode.hpp"
#include "nodes/rendering/PBRDrawableNode.hpp"
//#include "nodes/rendering/PointLightNode.hpp"

using namespace llengine;
using namespace llengine::internal;

void internal::register_builtin_nodes() {
    register_node_type<SpatialNode, Node>("spatial_node");
    register_node_type<CompleteSpatialNode, SpatialNode>("complete_spatial_node");
    register_node_type<CameraNode, SpatialNode>("camera_node");
    register_node_type<SpectatorCameraNode, SpatialNode>("spectator_camera_node");
    register_node_type<PBRDrawableNode, SpatialNode>("pbr_drawable_node");
    //register_node_type<PointLightNode>("point_light_node");

    register_node_type<GUINode, Node>("gui_node");
    register_node_type<ButtonNode, GUINode>("button_node");
    register_node_type<GUICanvas, SpatialNode>("gui_canvas");
    //register_node_type<TextNode>("text_node");
}