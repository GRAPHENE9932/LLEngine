#include "PhysicsServer.hpp"

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

    // Consider cuboid objects.
    for (const auto& cuboid : cuboid_objects) {
        if (cuboid->cylinder_intersects(player->cylinder)) {

            bool is_above;
            player->cylinder.position =
                cuboid->push_cylinder_out(player->cylinder, is_above);

            if (is_above) {
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
