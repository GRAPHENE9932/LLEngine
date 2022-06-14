#include <utility>
#include <array>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/geometric.hpp>
#include <glm/gtx/norm.hpp>

#include "structs/RoundedRectangle.hpp"
#include "PhysicsServer.hpp"

constexpr float ALLOWABLE_ERROR = 0.00005f;

/// Looks for the closest point in the given array.
/// @param points The array to find closest point from.
/// @param point Distance to the points in the array will be
/// measured between this point and a point from array.
/// @param amount Amount of first elements of the array to consider.
glm::vec2 comp_closest_point(const std::array<glm::vec2, 4>& points, glm::vec2 point, std::size_t amount) {
    glm::vec2 res_point = points[0];
    for (std::size_t i = 0; i < amount; i++)
        if (glm::distance2(points[i], point) < glm::distance2(res_point, point))
            res_point = points[i];

    return res_point;
}

void PhysicsServer::consider_bounds() {
    if (player->cylinder.position.x - player->cylinder.radius < left_bound)
        player->cylinder.position.x = left_bound + player->cylinder.radius;

    if (player->cylinder.position.x + player->cylinder.radius > right_bound)
        player->cylinder.position.x = right_bound - player->cylinder.radius;

    if (player->cylinder.position.z - player->cylinder.radius < bottom_bound)
        player->cylinder.position.z = bottom_bound + player->cylinder.radius;

    if (player->cylinder.position.z + player->cylinder.radius > top_bound)
        player->cylinder.position.z = top_bound - player->cylinder.radius;
}

void PhysicsServer::consider_floors() {
    glm::vec2 flat_player_position(player->cylinder.position.x, player->cylinder.position.z);
    for (const auto& floor : flat_floors) {
        if (floor->circle_over_floor(flat_player_position, player->cylinder.radius)) {
            if (player->cylinder.lowest_y() < floor->height) {
                player->cylinder.position.y = floor->height + player->cylinder.height * 0.5f;
                player->zero_out_vertical_velocity();
                player->landed();
            }
        }
    }
}

void PhysicsServer::consider_rectangular_walls() {
    for (const auto& wall : rectangular_walls) {
        if (wall->cylinder_intersects(player->cylinder)) {

            player->cylinder.position =
                wall->force_point_distance(player->cylinder.position, player->cylinder.radius);
        }
    }
}

void PhysicsServer::consider_cuboid_objects() {
    // These side cuboids are NEXT_TO the player cylinder.
    // To not call push_cylinder_out twice, the result of this
    // function for every cuboid stored in std::pair.
    static_vector<std::pair<CuboidObject*, glm::vec3>, 2> side_cuboids;

    for (const auto& cuboid : cuboid_objects) {
        if (cuboid->cylinder_intersects(player->cylinder)) {
            CuboidObject::RelativePos rel_pos {};
            glm::vec3 push_res {cuboid->push_cylinder_out(player->cylinder, rel_pos)};

            switch (rel_pos) {
            case CuboidObject::RelativePos::ABOVE:
                player->zero_out_vertical_velocity();
                player->landed();
                // Note that there is no break statement.
            case CuboidObject::RelativePos::UNDER:
                player->cylinder.position = push_res;
                break;
            case CuboidObject::RelativePos::NEXT_TO:
                side_cuboids.push_back(std::make_pair(cuboid.get(), push_res));
                break;
            }
        }
    }

    resolve_side_cuboids(side_cuboids);
}

void PhysicsServer::resolve_side_cuboids(const static_vector<std::pair<CuboidObject*, glm::vec3>, 2>& side_cuboids) {
    if (side_cuboids.size() == 2) {
        // Find out if one of the resolution cases 
        Cylinder player_in_case_1 {player->cylinder};
        player_in_case_1.position = side_cuboids[0].second;

        Cylinder player_in_case_2 {player->cylinder};
        player_in_case_2.position = side_cuboids[1].second;

        if (side_cuboids[0].first->horizontal_distance_from_cylinder(player_in_case_2) > -ALLOWABLE_ERROR) {
            player->cylinder = player_in_case_2;
        }
        else if (side_cuboids[1].first->horizontal_distance_from_cylinder(player_in_case_1) > -ALLOWABLE_ERROR) {
            player->cylinder = player_in_case_1;
        }
        else {
            // If we got there, then there is no valid resolution if we take every cuboid separately.
            // So, I'll place the player in the closest intersection point of two rounded
            // rectangles, constructed from cuboid's rectangle and player's cylinder radius.
            RoundedRectangle rect_1(side_cuboids[0].first->rect, player->cylinder.radius);
            RoundedRectangle rect_2(side_cuboids[1].first->rect, player->cylinder.radius);

            std::array<glm::vec2, 4> intersection_points;
            auto amount = rect_1.intersection_points(rect_2, intersection_points);

            assert(amount >= 1 && amount <= 4);
            glm::vec2 closest_point = comp_closest_point(intersection_points, player->cylinder.position, amount);

            player->cylinder.position.x = closest_point.x;
            player->cylinder.position.z = closest_point.y;
        }
    }
    else if (side_cuboids.size() == 1) {
        player->cylinder.position = side_cuboids[0].second;
    }
}

void PhysicsServer::update(float delta) {
    // Apply gravity.
    player->add_vertical_velocity(GRAVITY * delta);
    player->update(delta);    

    consider_bounds();
    consider_floors();
    consider_rectangular_walls();
    consider_cuboid_objects();

    player->update_camera(delta);
}

void PhysicsServer::set_player(std::shared_ptr<ControllableCylinder> player) {
    this->player = player;
}

void PhysicsServer::add_flat_floor(std::shared_ptr<FloorObject> flat_floor) {
    this->flat_floors.push_back(flat_floor);
}

void PhysicsServer::add_rectangular_wall(std::shared_ptr<RectangularWall> rectangular_wall) {
    this->rectangular_walls.push_back(rectangular_wall);
}

void PhysicsServer::add_cuboid_object(std::shared_ptr<CuboidObject> cuboid_object) {
    this->cuboid_objects.push_back(cuboid_object);
}

void PhysicsServer::set_bounds(float left_bound, float right_bound, float top_bound, float bottom_bound) {
    this->left_bound = left_bound;
    this->right_bound = right_bound;
    this->top_bound = top_bound;
    this->bottom_bound = bottom_bound;
}
