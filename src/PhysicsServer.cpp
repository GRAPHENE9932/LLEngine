#include <iostream>

#include <glm/geometric.hpp>

#include "structs/RoundedRectangle.hpp"
#include "PhysicsServer.hpp"

glm::vec2 closest_point(const std::array<glm::vec2, 4>& points, glm::vec2 point) {
    glm::vec2 result = points[0];
    for (std::size_t i = 0; i < 4; i++) {
        if (glm::distance(result, point) > glm::distance(points[i], point)) {
            result = points[i];
        }
    }
    return result;
}

void PhysicsServer::update(float delta) {
    // Apply gravity.
    player->add_vertical_velocity(GRAVITY * delta);

    player->update(delta);

    // Consider floors.
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

    // Consider bounds.
    if (player->cylinder.position.x - player->cylinder.radius < left_bound) {
        player->cylinder.position.x = left_bound + player->cylinder.radius;
    }
    if (player->cylinder.position.x + player->cylinder.radius > right_bound) {
        player->cylinder.position.x = right_bound - player->cylinder.radius;
    }
    if (player->cylinder.position.z - player->cylinder.radius < bottom_bound) {
        player->cylinder.position.z = bottom_bound + player->cylinder.radius;
    }
    if (player->cylinder.position.z + player->cylinder.radius > top_bound) {
        player->cylinder.position.z = top_bound - player->cylinder.radius;
    }

    // Consider rectangular walls.
    for (const auto& wall : rectangular_walls) {
        if (wall->cylinder_intersects(player->cylinder)) {

            player->cylinder.position =
                wall->force_point_distance(player->cylinder.position, player->cylinder.radius);
        }
    }

    std::vector<CuboidObject*> vec;
    // Consider cuboid objects.
    for (const auto& cuboid : cuboid_objects) {
        if (cuboid->cylinder_intersects(player->cylinder)) {
            vec.push_back(cuboid.get());
        }
    }

    if (vec.size() == 2) {
        CuboidObject::RelativePos is_above_1;
        glm::vec2 pushed_out_vec_1 {vec[0]->push_cylinder_out(player->cylinder, is_above_1).x, vec[0]->push_cylinder_out(player->cylinder, is_above_1).z};
        Cylinder pushed_out_1 {player->cylinder};
        pushed_out_1.position.x = pushed_out_vec_1.x;
        pushed_out_1.position.z = pushed_out_vec_1.y;

        CuboidObject::RelativePos is_above_2;
        glm::vec2 pushed_out_vec_2 {vec[1]->push_cylinder_out(player->cylinder, is_above_2).x, vec[1]->push_cylinder_out(player->cylinder, is_above_2).z};
        Cylinder pushed_out_2 {player->cylinder};
        pushed_out_2.position.x = pushed_out_vec_2.x;
        pushed_out_2.position.z = pushed_out_vec_2.y;

        if (!vec[1]->cylinder_intersects(pushed_out_1)) {
            player->cylinder = pushed_out_1;
        }
        else if (!vec[0]->cylinder_intersects(pushed_out_2)) {
            player->cylinder = pushed_out_2;
        }
        else {
            RoundedRectangle rect_1(vec[0]->rect, player->cylinder.radius);
            RoundedRectangle rect_2(vec[1]->rect, player->cylinder.radius);

            std::array<glm::vec2, 4> points {{
                {INFINITY, INFINITY},
                {INFINITY, INFINITY},
                {INFINITY, INFINITY},
                {INFINITY, INFINITY}
            }};
            rect_1.intersection_points(rect_2, points);
            auto point = closest_point(points, {player->cylinder.position.x, player->cylinder.position.z});
            player->cylinder.position.x = point.x;
            player->cylinder.position.z = point.y;
        }
    }
    else {
        for (int i = 0; i < vec.size(); i++) {
            CuboidObject::RelativePos is_above;
            player->cylinder.position =
                vec[i]->push_cylinder_out(player->cylinder, is_above);

            if (is_above == CuboidObject::RelativePos::NEXT_TO) {
                player->zero_out_vertical_velocity();
                player->landed();
            }
        }
    }

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
