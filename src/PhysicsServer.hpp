#pragma once

#include <vector>

#include "objects/ControllableCylinder.hpp"
#include "objects/FloorObject.hpp"
#include "objects/RectangularWall.hpp"

class PhysicsServer {
public:
    static constexpr float gravity = -0.98f;

    ControllableCylinder* player;
    std::vector<FloorObject> flat_floors;
    std::vector<RectangularWall> rectangular_walls;
    float left_bound, right_bound, top_bound, bottom_bound;

    void update(float delta);
};
