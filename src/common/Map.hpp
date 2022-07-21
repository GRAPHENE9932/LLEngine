#pragma once

#include <string>
#include <vector>
#include <cstdint>

#include <glm/vec3.hpp>

#include "objects/MovingLightBulb.hpp"
#include "RenderingServer.hpp"
#include "structs/Rect.hpp"
#include "PhysicsServer.hpp"

class Map {
public:
    struct MeshArgs {
        uint64_t id;
        std::string file;
    };
    struct TextureArgs {
        uint64_t id;
        std::string file;
    };
    struct TexDrawObjArgs {
        uint64_t id;
        uint64_t mesh_id;
        uint64_t texture_id;
        glm::vec3 position;
        glm::vec3 scale;
        glm::vec3 rotation;
    };
    struct UnshDrawObjArgs {
        uint64_t id;
        uint64_t mesh_id;
        glm::vec3 color;
        glm::vec3 position;
        glm::vec3 scale;
        glm::vec3 rotation;
    };
    struct FloorObjArgs {
        Rect rect;
        float height;
    };
    struct RectWallArgs {
        Rect rect;
    };
    struct CuboidObjArgs {
        Rect rect;
        float bottom_y, top_y;
    };
    struct PointLightArgs {
        uint64_t id;
        glm::vec3 position;
        glm::vec3 color;
        float diffuse_strength;
        float const_coeff;
        float linear_coeff;
        float quadratic_coeff;
    };
    struct MovingLightBulbArgs {
        uint64_t point_light_id;
        uint64_t drawable_object_id;
        std::vector<glm::vec3> path;
        float speed;
    };

    std::string name;

    std::vector<MeshArgs> meshes_args;
    std::vector<TextureArgs> textures_args;
    std::vector<TexDrawObjArgs> tex_draw_objects_args;
    std::vector<UnshDrawObjArgs> unsh_draw_objects_args;

    float left_bound, right_bound, front_bound, back_bound;
    std::vector<FloorObjArgs> flat_floors_args;
    std::vector<RectWallArgs> rect_walls_args;
    std::vector<CuboidObjArgs> cuboid_objects_args;

    std::vector<PointLightArgs> point_lights_args;
    std::vector<MovingLightBulbArgs> moving_light_bulbs_args;

    Map(std::string_view json_file_path);

    /// JSON must contain the object with these members:
    /// version = int (version of map specification. Now 1).
    /// name = string (name of the map).
    /// right_bound = float (global right (+X) bound).
    /// left_bound = float (global left (-X) bound).
    /// back_bound = float (global back (+Z) bound).
    /// front_bound = float (global front (-Z) bound).
    ///
    /// flat_floors[] (array of flat floors. Optional).
    /// rect = {pos_x = float, pos_z = float, size_x = float, size_z = float}
    ///
    /// meshes[] (array of meshes. Optional).
    /// id = int (ID of the mesh to specify in other places of the TOML).
    /// file = string (path to the mesh file).
    ///
    /// textures[] (array of textures. Optional).
    /// id = int (ID of the texture to specify in other places of the TOML).
    /// file = string (path to the texture file).
    ///
    /// textured_drawables[] (array of textured drawable objects. Optional).
    /// mesh_id = int
    /// texture_id = int
    /// position = {x = float, y = float, z = float} (default: 0, 0, 0).
    /// scale = {x = float, y = float, z = float} (default: 1, 1, 1).
    /// rotation = {x = float, y = float, z = float} (default: 0, 0, 0).
    ///
    /// unshaded_drawables[] (array of textured drawable objects. Optional).
    /// id = int (ID of the drawable object).
    /// mesh_id = int
    /// color = {r = float, g = float, b = float}
    /// position = {x = float, y = float, z = float} (default: 0, 0, 0).
    /// scale = {x = float, y = float, z = float} (default: 1, 1, 1).
    /// rotation = {x = float, y = float, z = float} (default: 0, 0, 0).
    ///
    /// rectangular_walls[] (array of rectangular walls of infinite height. Optional).
    /// rect = {pos_x = float, pos_z = float, size_x = float, size_z = float}
    ///
    /// cuboid_objects[] (array of physical cuboid objects. Optional).
    /// rect = {pos_x = float, pos_z = float, size_x = float, size_z = float}
    /// bottom_y = float
    /// top_y = float
    ///
    /// point_lights[] (array of point lights. Optional).
    /// id = int (ID of the point light).
    /// color = {r = float, g = float, b = float}.
    /// position = {x = float, y = float, z = float} (default: 0, 0, 0).
    /// diffuse_strength = float
    /// const_coeff = float
    /// linear_coeff = float
    /// quadratic_coeff = float
    ///
    /// moving_light_bulb[] (array of moving light bulbs. Optional).
    /// point_light_id = int
    /// drawable_object_id = int
    /// speed = float
    /// path = [
    ///     {x = float, y = float, z = float},
    ///     {x = float, y = float, z = float},
    ///     ...
    /// ]
    void from_json(std::string_view file_path);
    void set_map(RenderingServer& rs, PhysicsServer& ps,
                 std::vector<std::shared_ptr<MovingLightBulb>>& moving_bulbs);

private:
    void load_from_json(std::string_view file_path);
    void check_map();
};
