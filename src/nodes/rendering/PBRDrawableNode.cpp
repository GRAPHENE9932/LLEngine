#include "rendering/RenderingServer.hpp"
#include "nodes/rendering/PBRDrawableNode.hpp" // PBRDrawableNode
#include "nodes/rendering/CameraNode.hpp"
#include "math/AABB.hpp"
#include "rendering/shaders/PBRShaderManager.hpp"

#include <GL/glew.h>

#include <limits>

using namespace llengine;

constexpr std::string_view VERTEX_SHADOW_MAPPING_SHADER_TEXT =
    #include "shaders/misc/shadow_mapping.vert"
;
constexpr std::string_view FRAGMENT_SHADOW_MAPPING_SHADER_TEXT =
    #include "shaders/misc/shadow_mapping.frag"
;

static PBRShaderManager pbr_shader_manager;

PBRDrawableNode::PBRDrawableNode() = default;

PBRDrawableNode::PBRDrawableNode(
    const std::shared_ptr<Material>& material,
    const std::shared_ptr<const Mesh>& mesh
) : DrawableCompleteSpatialNode(), mesh(mesh), material(material) {}

static void draw_mesh(const Mesh& mesh, RenderingServer& rs) {
    mesh.bind_vao();

    if (mesh.is_indexed()) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.get_indices_id());
        glDrawElements(GL_TRIANGLES, mesh.get_amount_of_vertices(), mesh.get_indices_type(), 0);
    }
    else {
        glBindBuffer(GL_ARRAY_BUFFER, mesh.get_vertices_id());
        glDrawArrays(GL_TRIANGLES, 0, mesh.get_amount_of_vertices());
    }

    mesh.unbind_vao(true, true, true);
}

void PBRDrawableNode::draw() {
    RenderingServer& rs = get_rendering_server();

    // Do some checks.
    if (material->normal_map.has_value() &&
        (mesh->get_normals_id() == 0 || mesh->get_tangents_id() == 0)) {
        throw std::runtime_error(
            "Drawable node's material has a normal map, but "
            "its mesh doesn't have normals and/or tangents."
        );
    }

    // Use the shader.
    const glm::mat4 model_matrix = get_global_matrix();
    const glm::mat4 mvp = rs.get_current_camera_node().get_view_proj_matrix() * model_matrix;
    
    
    pbr_shader_manager.use_shader(
        rs, *material, mvp, model_matrix, rs.get_current_camera_node().get_global_position()
    );

    draw_mesh(*mesh, rs);
}

void PBRDrawableNode::draw_to_shadow_map() {
    RenderingServer& rs = get_rendering_server();

    const glm::mat4 model_matrix = get_global_matrix();
    const glm::mat4 mvp = rs.get_shadow_map().get_view_proj_matrix() * model_matrix;

    static Shader<"mvp"> shadow_mapping_shader(VERTEX_SHADOW_MAPPING_SHADER_TEXT, FRAGMENT_SHADOW_MAPPING_SHADER_TEXT);
    shadow_mapping_shader.use_shader();
    shadow_mapping_shader.set_mat4<"mvp">(mvp);

    draw_mesh(*mesh, rs);
}

GLuint PBRDrawableNode::get_program_id() const {
    RenderingServer& rs = get_rendering_server();
    return pbr_shader_manager.get_program_id(rs, *material);
}

void PBRDrawableNode::set_mesh(const std::shared_ptr<const Mesh>& mesh) {
    this->mesh = mesh;
}

void PBRDrawableNode::set_material(const std::shared_ptr<Material>& material) {
    this->material = material;
}

const Mesh& PBRDrawableNode::get_mesh() const {
    if (mesh == nullptr) {
        throw std::runtime_error("Failed to get mesh from PBRDrawableNode as it is null.");
    }

    return *mesh;
}

const Material& PBRDrawableNode::get_material() const {
    if (material == nullptr) {
        throw std::runtime_error("Failed to get material from PBRDrawableNode as it is null.");
    }

    return *material;
}

[[nodiscard]] static AABB model_space_to_world_space_aabb(const AABB& model_space_aabb, const glm::mat4& model_matrix) {
    AABB result {
        {
            std::numeric_limits<float>::lowest(),
            std::numeric_limits<float>::lowest(),
            std::numeric_limits<float>::lowest()
        },
        {
            std::numeric_limits<float>::max(),
            std::numeric_limits<float>::max(),
            std::numeric_limits<float>::max()
        }
    };

    for (std::uint8_t i = 0; i < 8; i++) {
        glm::vec3 world_space_vertex = model_matrix * glm::vec4(model_space_aabb.get_vertex(i), 1.0f);
        
        for (std::size_t j = 0; j < 3; j++) {
            if (result.point_max[j] < world_space_vertex[j]) {
                result.point_max[j] = world_space_vertex[j];
            }
            if (result.point_min[j] > world_space_vertex[j]) {
                result.point_min[j] = world_space_vertex[j];
            }
        }
    }

    return result;
}

[[nodiscard]] bool PBRDrawableNode::is_outside_the_frustum(const Frustum& frustum) const {
    if (mesh == nullptr) {
        return false;
    }

    AABB aabb = model_space_to_world_space_aabb(mesh->get_aabb(), get_global_matrix());
    return !frustum.is_aabb_on_frustum(aabb);
}

void PBRDrawableNode::copy_to(Node& node) const {
    CompleteSpatialNode::copy_to(node);

    PBRDrawableNode& pbr_node = dynamic_cast<PBRDrawableNode&>(node);
    pbr_node.material = material;
    pbr_node.mesh = mesh;
}

std::unique_ptr<Node> PBRDrawableNode::copy() const {
    std::unique_ptr<Node> result = std::make_unique<PBRDrawableNode>();
    copy_to(*result);
    return result;
}