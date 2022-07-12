#pragma once

#include "objects/TexturedDrawableObject.hpp"
#include "objects/UnshadedDrawableObject.hpp"
#include "objects/FloorObject.hpp"
#include "objects/RectangularWall.hpp"
#include "objects/CuboidObject.hpp"
#include "objects/MovingLightBulb.hpp"

#include <string>
#include <memory>

class Map {
public:
    std::string name;

    std::vector<std::shared_ptr<Mesh>> meshes;
    std::vector<std::shared_ptr<Texture>> textures;
    std::vector<std::shared_ptr<TexturedDrawableObject>> tex_draw_objects;
    std::vector<std::shared_ptr<UnshadedDrawableObject>> unsh_draw_objects;

    float left_bound, right_bound, front_bound, back_bound;
    std::vector<std::shared_ptr<FloorObject>> flat_floors;
    std::vector<std::shared_ptr<RectangularWall>> rect_walls;
    std::vector<std::shared_ptr<CuboidObject>> cuboid_objects;

    std::vector<std::shared_ptr<PointLight>> point_lights;
    std::vector<std::shared_ptr<MovingLightBulb>> moving_light_bulbs;

    Map(std::string_view toml_file_path);

    /// TOML must contain this data:
    /// version = int (version of map specification. Now 1).
    /// name = string (name of the map).
    /// right_bound = float (global right (+X) bound).
    /// left_bound = float (global left (-X) bound).
    /// back_bound = float (global back (+Z) bound).
    /// front_bound = float (global front (-Z) bound).
    ///
    /// [[flat_floors]] (array of flat floors. Optional).
    /// rect = {pos_x = float, pos_z = float, size_x = float, size_z = float}
    ///
    /// [[meshes]] (array of meshes. Optional).
    /// id = int (ID of the mesh to specify in other places of the TOML).
    /// file = string (path to the mesh file).
    ///
    /// [[textures]] (array of textures. Optional).
    /// id = int (ID of the texture to specify in other places of the TOML).
    /// file = string (path to the texture file).
    ///
    /// [[textured_drawables]] (array of textured drawable objects. Optional).
    /// mesh_id = int
    /// texture_id = int
    /// position = {x = float, y = float, z = float} (default: 0, 0, 0).
    /// scale = {x = float, y = float, z = float} (default: 1, 1, 1).
    /// rotation = {x = float, y = float, z = float} (default: 0, 0, 0).
    ///
    /// [[unshaded_drawables]] (array of textured drawable objects. Optional).
    /// id = int (ID of the drawable object).
    /// mesh_id = int
    /// color = {r = float, g = float, b = float}
    /// position = {x = float, y = float, z = float} (default: 0, 0, 0).
    /// scale = {x = float, y = float, z = float} (default: 1, 1, 1).
    /// rotation = {x = float, y = float, z = float} (default: 0, 0, 0).
    ///
    /// [[rectangular_walls]] (array of rectangular walls of infinite height. Optional).
    /// rect = {pos_x = float, pos_z = float, size_x = float, size_z = float}
    ///
    /// [[cuboid_objects]] (array of physical cuboid objects. Optional).
    /// rect = {pos_x = float, pos_z = float, size_x = float, size_z = float}
    /// bottom_y = float
    /// top_y = float
    ///
    /// [[point_lights]] (array of point lights. Optional).
    /// id = int (ID of the point light).
    /// color = {r = float, g = float, b = float}.
    /// position = {x = float, y = float, z = float} (default: 0, 0, 0).
    /// diffuse_strength = float
    /// const_coeff = float
    /// linear_coeff = float
    /// quadratic_coeff = float
    ///
    /// [[moving_light_bulb]] (array of moving light bulbs. Optional).
    /// point_light_id = int
    /// drawable_object_id = int
    /// speed = float
    /// path = [
    ///     {x = float, y = float, z = float},
    ///     {x = float, y = float, z = float},
    ///     ...
    /// ]
    void from_toml(std::string_view file_path);
};
