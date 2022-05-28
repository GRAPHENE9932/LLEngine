#include <fstream>
#include <unordered_map>

#include "Map.hpp"

Map::Map(const std::string& file_path) {
    from_file(file_path);
}

void map_parse_error(const std::string& err_text, const std::string& file_path) {
    throw std::runtime_error(
        "Failed to read the map file.\n" +
        err_text + "\nFile path: " + file_path
    );
}

constexpr const char* MESHES_PATH = "res/meshes/";
constexpr const char* TEXTURES_PATH = "res/textures/";

// Specification:
// n <name> // Name of the map.
// m <index> <file_name> // Mesh, its index and file name.
// t <index> <file_name> // Texture, its index and file name.
// o <mesh_index> <texture_index> <position.x> <y> <z> <scale.x> <y> <z> <euler_rot.x> <y> <z> // Textured object.
// ff <rect.pos.x> <y> <rect.size.x> <y> <height> // Flat floor and its position and height.
// b <left_bound> <right_bound> <bottom_bound> <top_bound> // Global vertical bounds.
// rw <rect.pos.x> <y> <rect.size.x> <y> // Rectangular wall, its coordinates and sizes.
// co <rect.pos.x> <y> <rect.size.x> <y> <lowest_y> <highest_y> // Cuboid physical object.
void Map::from_file(const std::string& file_path) {
    meshes.clear();
    textures.clear();
    tex_draw_objects.clear();
    unsh_draw_objects.clear();
    flat_floors.clear();
    rect_walls.clear();
    cuboid_objects.clear();

    std::unordered_map<uint16_t, std::shared_ptr<Mesh>> tmp_meshes;
    std::unordered_map<uint16_t, std::shared_ptr<Texture>> tmp_textures;

    std::ifstream stream(file_path, std::ios::in);
    if (!stream)
        map_parse_error("File does not exist or inaccessible.", file_path);

    while (true) {
        std::string first_token;
        stream >> first_token;

        if (!stream)
            break;

        if (first_token == "n") {
            stream >> name;
        }
        else if (first_token == "m") {
            uint16_t index;
            std::string file_name;
            stream >> index >> file_name;
            tmp_meshes.insert(std::make_pair(
                index, std::make_shared<Mesh>(MESHES_PATH + file_name)
            ));
        }
        else if (first_token == "t") {
            uint16_t index;
            std::string file_name;
            stream >> index >> file_name;
            tmp_textures.insert(std::make_pair(
                index, std::make_shared<Texture>(TEXTURES_PATH + file_name)
            ));
        }
        else if (first_token == "o") {
            glm::vec3 position, scale, euler_rot;
            uint16_t mesh_index, texture_index;
            stream >> mesh_index >> texture_index >> position.x >>
                position.y >> position.z >> scale.x >> scale.y >> scale.z >>
                euler_rot.x >> euler_rot.y >> euler_rot.z;
            auto new_obj = std::make_shared<TexturedDrawableObject>(
                tmp_textures.at(texture_index),
                tmp_meshes.at(mesh_index)
            );
            new_obj->translation = position;
            new_obj->scale = scale;
            new_obj->rotation = glm::quat(euler_rot);
            tex_draw_objects.push_back(new_obj);
        }
        else if (first_token == "ff") {
            Rect rect;
            float height;
            stream >> rect.position.x >> rect.position.y >> rect.size.x >> rect.size.y >> height;

            auto new_floor = std::make_shared<FloorObject>(rect, height);
            flat_floors.push_back(new_floor);
        }
        else if (first_token == "b") {
            stream >> left_bound >> right_bound >> bottom_bound >> top_bound;
        }
        else if (first_token == "rw") {
            Rect rect;
            stream >> rect.position.x >> rect.position.y >> rect.size.x >> rect.size.y;
            auto new_wall = std::make_shared<RectangularWall>(rect);
            rect_walls.push_back(new_wall);
        }
        else if (first_token == "co") {
            Rect rect;
            float bottom_y, top_y;
            stream >> rect.position.x >> rect.position.y >> rect.size.x >> rect.size.y >>
                bottom_y >> top_y;
            auto new_obj = std::make_shared<CuboidObject>(rect, bottom_y, top_y);
            cuboid_objects.push_back(new_obj);
        }
        else {
            map_parse_error("Unknown first token.", file_path);
        }
    }
}
