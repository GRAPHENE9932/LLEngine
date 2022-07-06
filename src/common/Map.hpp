#pragma once

#include "objects/TexturedDrawableObject.hpp"
#include "objects/UnshadedDrawableObject.hpp"
#include "objects/FloorObject.hpp"
#include "objects/RectangularWall.hpp"
#include "objects/CuboidObject.hpp"

#include <string>
#include <memory>

class Map {
public:
    std::string name;

    std::vector<std::shared_ptr<Mesh>> meshes;
    std::vector<std::shared_ptr<Texture>> textures;
    std::vector<std::shared_ptr<TexturedDrawableObject>> tex_draw_objects;
    std::vector<std::shared_ptr<UnshadedDrawableObject>> unsh_draw_objects;

    float left_bound, right_bound, bottom_bound, top_bound;
    std::vector<std::shared_ptr<FloorObject>> flat_floors;
    std::vector<std::shared_ptr<RectangularWall>> rect_walls;
    std::vector<std::shared_ptr<CuboidObject>> cuboid_objects;

    Map(const std::string& file_path);

    void from_file(const std::string& file_path);
};
