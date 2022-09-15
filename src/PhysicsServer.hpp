/*#pragma once

#include <vector>
#include <memory>

#include "structs/static_vector.hpp"
#include "nodes/core/physics/ControllableCylinder.hpp"
#include "nodes/core/physics/FloorObject.hpp"
#include "nodes/core/physics/RectangularWall.hpp"
#include "nodes/core/physics/CuboidObject.hpp"

class PhysicsServer {
public:
    static constexpr float GRAVITY = -98.0f;

    void update(float delta);
    void set_player(std::shared_ptr<ControllableCylinder> player);
    void add_flat_floor(std::shared_ptr<FloorObject> flat_floor);
    void add_rectangular_wall(std::shared_ptr<RectangularWall> rectangular_wall);
    void add_cuboid_object(std::shared_ptr<CuboidObject> cuboid_object);
    void set_bounds(float left_bound, float right_bound, float top_bound, float bottom_bound);

private:
    std::shared_ptr<ControllableCylinder> player;
    std::vector<std::shared_ptr<FloorObject>> flat_floors;
    std::vector<std::shared_ptr<RectangularWall>> rectangular_walls;
    std::vector<std::shared_ptr<CuboidObject>> cuboid_objects;
    float left_bound, right_bound, top_bound, bottom_bound;

    void consider_bounds();

    void consider_floors();

    void consider_rectangular_walls();

    void consider_cuboid_objects();

    struct CuboidAndRes {
        CuboidObject* cuboid = nullptr;
        glm::vec3 resolution;
    };
    void resolve_side_cuboids(const static_vector<CuboidAndRes, 2>& side_cuboids);
    void resolve_base_cuboid(const CuboidAndRes& base_cuboid,
            const static_vector<CuboidAndRes, 2>& side_cuboids);
};
*/
