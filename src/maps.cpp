#include <memory>

#include "utils/utils.hpp"
#include "common/Mesh.hpp"
#include "objects/TexturedDrawableObject.hpp"
#include "maps.hpp"

void maps::prepare_map_close(RenderingServer& rs, PhysicsServer& ps) {
    GLuint map_texture = utils::load_dds("res/textures/map_close.dds");
    GLuint box_texture = utils::load_dds("res/textures/wooden_box.dds");
    GLuint texture_id = utils::load_dds("res/textures/crosshair.dds");

    std::shared_ptr<Mesh> map_mesh = std::make_shared<Mesh>("res/meshes/map_close.obj");
    std::shared_ptr<Mesh> box_mesh = std::make_shared<Mesh>("res/meshes/cube.obj");

    TexturedDrawableObject* map = new TexturedDrawableObject(map_texture, map_mesh);
    map->translation = glm::vec3(0.0f, 0.0f, 0.0f);
    map->rotation = glm::quat();
    map->scale = glm::vec3(1.0f, 1.0f, 1.0f);
    rs.add_textured_drawable_object(map);

    TexturedDrawableObject* wooden_box_1 = new TexturedDrawableObject(box_texture, box_mesh);
    wooden_box_1->translation = glm::vec3(2.5f, 1.5f, -5.0f);
    wooden_box_1->rotation = glm::quat();
    wooden_box_1->scale = glm::vec3(3.0f, 3.0f, 3.0f);
    rs.add_textured_drawable_object(wooden_box_1);

    TexturedDrawableObject* wooden_box_2 = new TexturedDrawableObject(box_texture, box_mesh);
    wooden_box_2->translation = glm::vec3(13.5f, 1.5f, -8.48912f);
    wooden_box_2->rotation = glm::quat();
    wooden_box_2->scale = glm::vec3(3.0f, 3.0f, 3.0f);
    rs.add_textured_drawable_object(wooden_box_2);

    TexturedDrawableObject* wooden_box_3 = new TexturedDrawableObject(box_texture, box_mesh);
    wooden_box_3->translation = glm::vec3(-18.5284f, 1.5f, -15.0f);
    wooden_box_3->rotation = glm::quat();
    wooden_box_3->scale = glm::vec3(3.0f, 3.0f, 3.0f);
    rs.add_textured_drawable_object(wooden_box_3);

    TexturedDrawableObject* wooden_box_4 = new TexturedDrawableObject(box_texture, box_mesh);
    wooden_box_4->translation = glm::vec3(-18.5284f, 1.5f, -12.0f);
    wooden_box_4->rotation = glm::quat();
    wooden_box_4->scale = glm::vec3(3.0f, 3.0f, 3.0f);
    rs.add_textured_drawable_object(wooden_box_4);

    TexturedDrawableObject* wooden_box_5 = new TexturedDrawableObject(box_texture, box_mesh);
    wooden_box_5->translation = glm::vec3(-18.5284f, 4.5f, -15.0f);
    wooden_box_5->rotation = glm::quat();
    wooden_box_5->scale = glm::vec3(3.0f, 3.0f, 3.0f);
    rs.add_textured_drawable_object(wooden_box_5);

    TexturedDrawableObject* wooden_box_6 = new TexturedDrawableObject(box_texture, box_mesh);
    wooden_box_6->translation = glm::vec3(-8.5f, 1.5f, 13.4519f);
    wooden_box_6->rotation = glm::quat();
    wooden_box_6->scale = glm::vec3(3.0f, 3.0f, 3.0f);
    rs.add_textured_drawable_object(wooden_box_6);

    TexturedDrawableObject* wooden_box_7 = new TexturedDrawableObject(box_texture, box_mesh);
    wooden_box_7->translation = glm::vec3(-8.5f, 4.5f, 13.4519f);
    wooden_box_7->rotation = glm::quat();
    wooden_box_7->scale = glm::vec3(3.0f, 3.0f, 3.0f);
    rs.add_textured_drawable_object(wooden_box_7);

    rs.light_direction = glm::vec3(0.4f, 0.5f, 1.0f);

    ImageObject2D* crosshair = new ImageObject2D(texture_id, Rect({0.0f, 0.0f}, {0.1f, 0.1f}), true);
    rs.add_image_2d_object(crosshair);

    ps.flat_floors.push_back(
        FloorObject(Rect({-20.0f, -20.0f}, {40.0f, 40.0f}), 0.0f)
    );
    ps.rectangular_walls.push_back(RectangularWall(Rect({-12.0f, 4.0f}, {8.0f, 8.0f})));
    ps.rectangular_walls.push_back(RectangularWall(Rect({4.0f, 4.0f}, {8.0f, 8.0f})));
    ps.rectangular_walls.push_back(RectangularWall(Rect({4.0f, -12.0f}, {8.0f, 8.0f})));
    ps.rectangular_walls.push_back(RectangularWall(Rect({-12.0f, -12.0f}, {8.0f, 8.0f})));
    ps.cuboid_objects.push_back(CuboidObject(Rect({1.0f, -6.5f}, {3.0f, 3.0f}), 0.0f, 3.0f));
    ps.cuboid_objects.push_back(CuboidObject(Rect({12.0f, -9.98912f}, {3.0f, 3.0f}), 0.0f, 3.0f));
    ps.cuboid_objects.push_back(CuboidObject(Rect({-20.0284f, -16.5f}, {3.0f, 3.0f}), 0.0f, 3.0f));
    ps.cuboid_objects.push_back(CuboidObject(Rect({-20.0284f, -13.5f}, {3.0f, 3.0f}), 0.0f, 3.0f));
    ps.cuboid_objects.push_back(CuboidObject(Rect({-20.0284f, -16.5f}, {3.0f, 3.0f}), 3.0f, 6.0f));
    ps.cuboid_objects.push_back(CuboidObject(Rect({-10.0f, 11.9519f}, {3.0f, 3.0f}), 0.0f, 3.0f));
    ps.cuboid_objects.push_back(CuboidObject(Rect({-10.0f, 11.9519f}, {3.0f, 3.0f}), 3.0f, 6.0f));
    ps.left_bound = -20.0f;
    ps.right_bound = 20.0f;
    ps.bottom_bound = -20.0f;
    ps.top_bound = 20.0f;
}
