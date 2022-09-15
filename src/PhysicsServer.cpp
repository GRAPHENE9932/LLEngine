/*#include <utility>
#include <array>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/geometric.hpp>
#include <glm/gtx/norm.hpp>

#include "structs/RoundedRectangle.hpp"
#include "PhysicsServer.hpp"

constexpr float ALLOWABLE_ERROR = 0.00005f;
*/
/**
 * +------+
 * |      |
 * |      |X
 * +------+X <- player
 *   +----|-|
 *   |     ^|
 *   |  MIN_LEDGE_LENGTH
 *   +------+
 * The minimum ledge length when player still can stay on it.
 */
/*constexpr float MIN_LEDGE_LENGTH = 0.05f;

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
    const glm::vec2 flat_player_position {player->cylinder.position.x, player->cylinder.position.z};
    for (const auto& floor : flat_floors) {
        if (floor->circle_over_floor(flat_player_position, player->cylinder.radius)) {
            if (player->cylinder.lowest_y() < floor->height) {
                player->cylinder.position.y = floor->height + player->cylinder.height * 0.5f;
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
    static_vector<CuboidAndRes, 2> side_cuboids;
    // The cuboid player standing on.
    CuboidAndRes base_cuboid {};

    for (const auto& cuboid : cuboid_objects) {
        if (cuboid->cylinder_intersects(player->cylinder)) {
            CuboidObject::RelativePos rel_pos {};
            const glm::vec3 push_res {cuboid->push_cylinder_out(player->cylinder, rel_pos)};

            switch (rel_pos) {
            case CuboidObject::RelativePos::ABOVE:
                base_cuboid = {cuboid.get(), push_res};
                break;
            case CuboidObject::RelativePos::UNDER:
                break;
            case CuboidObject::RelativePos::NEXT_TO:
                if (side_cuboids.size() < 2)
                    side_cuboids.push_back({cuboid.get(), push_res});
                break;
            }
        }
    }

    resolve_base_cuboid(base_cuboid, side_cuboids);
    resolve_side_cuboids(side_cuboids);
}

void PhysicsServer::resolve_side_cuboids(const static_vector<CuboidAndRes, 2>& side_cuboids) {
    if (side_cuboids.size() == 2) {
        // Find out if one of the resolution cases also automaticallu
        // resolves other cuboid's resolution.
        Cylinder player_in_case_1 {player->cylinder};
        player_in_case_1.position = side_cuboids[0].resolution;

        Cylinder player_in_case_2 {player->cylinder};
        player_in_case_2.position = side_cuboids[1].resolution;

        if (side_cuboids[0].cuboid->horizontal_distance_from_cylinder(player_in_case_2) > -ALLOWABLE_ERROR) {
            player->cylinder = player_in_case_2;
        }
        else if (side_cuboids[1].cuboid->horizontal_distance_from_cylinder(player_in_case_1) > -ALLOWABLE_ERROR) {
            player->cylinder = player_in_case_1;
        }
        else {
            // If we got there, then there is no valid resolution if we take every cuboid separately.
            // So, I'll place the player in the closest intersection point of two rounded
            // rectangles, constructed from cuboid's rectangle and player's cylinder radius.
            const RoundedRectangle& rect_1 {side_cuboids[0].cuboid->get_rounded_rect(player->cylinder.radius)};
            const RoundedRectangle& rect_2 {side_cuboids[1].cuboid->get_rounded_rect(player->cylinder.radius)};

            std::array<glm::vec2, 4> intersection_points;
            auto amount = rect_1.intersection_points(rect_2, intersection_points);

            assert(amount >= 1 && amount <= 4);
            glm::vec2 closest_point = comp_closest_point(intersection_points, player->cylinder.position, amount);

            player->cylinder.position.x = closest_point.x;
            player->cylinder.position.z = closest_point.y;
        }
    }
    else if (side_cuboids.size() == 1) {
        player->cylinder.position = side_cuboids[0].resolution;
    }
}

void PhysicsServer::resolve_base_cuboid(const CuboidAndRes& base_cuboid,
        const static_vector<CuboidAndRes, 2>& side_cuboids) {
    if (base_cuboid.cuboid == nullptr)
        return;

    if (player->get_vertical_velocity() > 0.0f)
        return;
    
    if (side_cuboids.size() == 0) {
        player->cylinder.position = base_cuboid.resolution;
        player->landed();
        return;
    }

    // Calculate distances between side cuboids resolution resulting horizontal positions
    // and base cuboid resolution resulting horizontal position. If the minimum distance
    // is greater than MIN_LEDGE_LENGTH, then place base cuboid in priority and
    // let the player stay on it. Otherwise, ignore it.
    const glm::vec2 player_hor_pos {player->cylinder.position.x, player->cylinder.position.z};

    const RoundedRectangle& rounded_rect_base {base_cuboid.cuboid->get_rounded_rect(player->cylinder.radius)};
    const glm::vec2 hor_pos_base {rounded_rect_base.closest_point(player_hor_pos)};

    float min_distance2 {std::numeric_limits<float>().infinity()};
    for (uint8_t i = 0; i < side_cuboids.size(); i++) {
        const RoundedRectangle& cur_rounded_rect {side_cuboids[i].cuboid->get_rounded_rect(player->cylinder.radius)};
        const glm::vec2 cur_hor_pos {cur_rounded_rect.closest_point(player_hor_pos)};
        const float cur_distance2 {glm::distance2(hor_pos_base, cur_hor_pos)};
        if (cur_distance2 < min_distance2)
            min_distance2 = cur_distance2;
    }
    
    if (min_distance2 > MIN_LEDGE_LENGTH * MIN_LEDGE_LENGTH) {
        player->cylinder.position = base_cuboid.resolution;
        player->landed();
    }
}

void PhysicsServer::update(float delta) {
    // Apply gravity.
    player->update(delta);
    player->add_vertical_velocity(GRAVITY * delta);

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
*/