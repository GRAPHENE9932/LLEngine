#include <map>
#include <memory>
#include <fstream>
#include <sstream>
#include <optional>
#include <algorithm>

#include <nlohmann/json.hpp>

#include "Map.hpp"
#include "common/Mesh.hpp"
#include "common/KTXTexture.hpp"
#include "objects/TexturedDrawableObject.hpp"
#include "objects/UnshadedDrawableObject.hpp"

Map::Map(std::string_view json_file_path) {
    from_json(json_file_path);
}

using json = nlohmann::json;

constexpr const char* MESHES_PATH = "res/meshes/";
constexpr const char* TEXTURES_PATH = "res/textures/";

constexpr glm::vec3 DEFAULT_POS {0.0f, 0.0f, 0.0f};
constexpr glm::vec3 DEFAULT_SCALE {1.0f, 1.0f, 1.0f};
constexpr glm::vec3 DEFAULT_ROT {0.0f, 0.0f, 0.0f};

void handle_root_elements(Map& map, const json& root_json) {
    const uint64_t version {root_json.at("version").get<uint64_t>()};
    if (version != 1)
        throw std::runtime_error("Invalid version. ");

    root_json.at("name").get_to(map.name);
    root_json.at("left_bound").get_to(map.left_bound);
    root_json.at("right_bound").get_to(map.right_bound);
    root_json.at("back_bound").get_to(map.back_bound);
    root_json.at("front_bound").get_to(map.front_bound);
}

glm::vec3 vec3_from_json(const json& local_json, const bool is_color = false) {
    return {
        local_json.at(is_color ? "r" : "x").get<double>(),
        local_json.at(is_color ? "g" : "y").get<double>(),
        local_json.at(is_color ? "b" : "z").get<double>()
    };
}

glm::vec3 vec3_from_json(const json& upper_json, std::string_view key,
                         const glm::vec3& def, const bool is_color = false) {
    try {
        return vec3_from_json(upper_json.at(std::string(key)), is_color);
    }
    catch (...) {
        return def;
    }
}

// from_json function must be in the same namespace as
// the object that I try to convert (glm::vec3).
namespace glm {
    void from_json(const json& local_json, glm::vec3& vec) {
        vec = vec3_from_json(local_json);
    }
}

void from_json(const json& local_json, Rect& rect) {
    rect.position.x = local_json.at("pos_x").get<double>();
    rect.position.y = local_json.at("pos_z").get<double>();
    rect.size.x = local_json.at("size_x").get<double>();
    rect.size.y = local_json.at("size_z").get<double>();
}

void from_json(const json& local_json, Map::FloorObjArgs& floor) {
    local_json.at("rect").get_to(floor.rect);
    local_json.at("height").get_to(floor.height);
}

void from_json(const json& local_json, Map::MeshArgs& mesh) {
    local_json.at("id").get_to(mesh.id);
    local_json.at("file").get_to(mesh.file);
}

void from_json(const json& local_json, Map::TextureArgs& texture) {
    local_json.at("id").get_to(texture.id);
    local_json.at("file").get_to(texture.file);
}

void from_json(const json& local_json, Map::RectWallArgs& wall) {
    local_json.at("rect").get_to(wall.rect);
}

void from_json(const json& local_json, Map::CuboidObjArgs& object) {
    local_json.at("rect").get_to(object.rect);
    local_json.at("bottom_y").get_to(object.bottom_y);
    local_json.at("top_y").get_to(object.top_y);
}

void from_json(const json& local_json, Map::PointLightArgs& light) {
    local_json.at("id").get_to(light.id);
    light.position = vec3_from_json(local_json, "position", DEFAULT_POS);
    light.color = vec3_from_json(local_json.at("color"), true);
    local_json.at("diffuse_strength").get_to(light.diffuse_strength);
    local_json.at("const_coeff").get_to(light.const_coeff);
    local_json.at("linear_coeff").get_to(light.linear_coeff);
    local_json.at("quadratic_coeff").get_to(light.quadratic_coeff);
}

void from_json(const json& local_json, Map::MovingLightBulbArgs& bulb) {
    local_json.at("point_light_id").get_to(bulb.point_light_id);
    local_json.at("drawable_object_id").get_to(bulb.drawable_object_id);
    local_json.at("path").get_to(bulb.path);
    local_json.at("speed").get_to(bulb.speed);
}

void from_json(const json& local_json, Map::TexDrawObjArgs& object) {
    local_json.at("id").get_to(object.id);
    local_json.at("mesh_id").get_to(object.mesh_id);
    local_json.at("texture_id").get_to(object.texture_id);
    object.position = vec3_from_json(local_json, "position", DEFAULT_POS);
    object.scale = vec3_from_json(local_json, "scale", DEFAULT_SCALE);
    object.rotation = vec3_from_json(local_json, "rotation", DEFAULT_ROT);
}

void from_json(const json& local_json, Map::UnshDrawObjArgs& object) {
    local_json.at("id").get_to(object.id);
    local_json.at("mesh_id").get_to(object.mesh_id);
    object.color = vec3_from_json(local_json.at("color"), true);
    object.position = vec3_from_json(local_json, "position", DEFAULT_POS);
    object.scale = vec3_from_json(local_json, "scale", DEFAULT_SCALE);
    object.rotation = vec3_from_json(local_json, "rotation", DEFAULT_ROT);
}

void handle_flat_floors(Map& map, const json& root_json) {
    root_json.at("flat_floors").get_to(map.flat_floors_args);
}

std::vector<Map::MeshArgs>
handle_meshes(const json& root_json) {
    return root_json.at("meshes").get<std::vector<Map::MeshArgs>>();
}

std::vector<Map::TextureArgs>
handle_textures(const json& root_json) {
    return root_json.at("textures").get<std::vector<Map::TextureArgs>>();
}

template<typename I, typename T>
std::optional<T> find_in_vector(const std::vector<std::pair<I, T>> vector, I index) {
    auto iter {std::find_if(
        vector.begin(),
        vector.end(),
        [&index](const std::pair<I, T>& element) {
            return element.first == index;
        }
    )};

    if (iter == vector.end())
        return {};

    return std::make_optional((*iter).second);
}

void handle_textured_drawables(Map& map, const json& root_json) {
    root_json.at("textured_drawables").get_to(map.tex_draw_objects_args);
}
void Map::from_json(std::string_view file_path) {
    load_from_json(file_path);
    check_map();
}

void Map::load_from_json(std::string_view file_path) {
    const std::string file_path_str {file_path};
    std::ifstream stream(file_path_str);

    json root_json;
    stream >> root_json;

    root_json.at("meshes").get_to(meshes_args);
    root_json.at("textures").get_to(textures_args);
    root_json.at("textured_drawables").get_to(tex_draw_objects_args);
    root_json.at("unshaded_drawables").get_to(unsh_draw_objects_args);
    root_json.at("left_bound").get_to(left_bound);
    root_json.at("right_bound").get_to(right_bound);
    root_json.at("front_bound").get_to(front_bound);
    root_json.at("back_bound").get_to(back_bound);
    root_json.at("flat_floors").get_to(flat_floors_args);
    root_json.at("rectangular_walls").get_to(rect_walls_args);
    root_json.at("cuboid_objects").get_to(cuboid_objects_args);
    root_json.at("point_lights").get_to(point_lights_args);
    root_json.at("moving_light_bulbs").get_to(moving_light_bulbs_args);
}

void check_file(std::string_view file_path_str, std::string_view extension) {
    using std::string_literals::operator""s;
    namespace fs = std::filesystem;

    fs::path file_path(file_path_str);

    if (file_path.extension() != extension) {
        std::stringstream message;
        message << "File \"" << file_path << "\" doesn't have the \"" <<
                extension << "\" extension.";
        throw std::runtime_error(message.str());
    }

    if (!fs::exists(file_path)) {
        std::stringstream message;
        message << "File " << file_path << " doesn't exist.";
        throw std::runtime_error(message.str());
    }
}

/// Warning: sorts the given vector.
/// Returns true if the check was successful (no duplicates).
template<typename T>
bool check_for_id_duplicates(std::vector<T>& vec) {
    std::sort(
        vec.begin(),
        vec.end(),
        [](const T& left, const T& right) {
            return left.id > right.id;
        }
    );
    return std::adjacent_find(
        vec.begin(),
        vec.end(),
        [](const T& left, const T& right) {
            return left.id == right.id;
        }
    ) == vec.end();
}

/// Warning: sorts the given vector.
/// Returns true if the check was successful (no duplicates).
template<typename T>
bool check_for_duplicates(std::vector<T>& vec) {
    std::sort(vec.begin(), vec.end());
    return std::adjacent_find(vec.begin(), vec.end()) == vec.end();
}

/// Returns true if the check was successful.
template<typename T, typename U, typename F1, typename F2>
bool validate_id(const std::vector<T>& checked_objects, const std::vector<U>& ids_source,
                 const F1&& get_checked_objects_id, const F2&& get_sources_ids) {
    for (const auto& checked_object : checked_objects) {
        bool contains_id = std::find_if(
            ids_source.begin(),
            ids_source.end(),
            [&checked_object, &get_checked_objects_id, &get_sources_ids](const auto& element) {
                return get_checked_objects_id(checked_object) == get_sources_ids(element);
            }
        ) != ids_source.end();
        if (!contains_id)
            return false;
    }

    return true;
}

void Map::check_map() {
    // Check for ID duplicates.
    if (!check_for_id_duplicates(meshes_args))
        throw std::runtime_error("Meshes have duplicate IDs.");
    if (!check_for_id_duplicates(textures_args))
        throw std::runtime_error("Textures have duplicate IDs.");
    std::vector<uint64_t> drawable_ids;
    drawable_ids.reserve(tex_draw_objects_args.size() + unsh_draw_objects_args.size());
    for (const auto& obj : tex_draw_objects_args)
        drawable_ids.push_back(obj.id);
    for (const auto& obj : unsh_draw_objects_args)
        drawable_ids.push_back(obj.id);
    if (!check_for_duplicates(drawable_ids))
        throw std::runtime_error("Drawable objects have duplicate IDs.");

    // Check if IDs are valid.
    if (!validate_id(tex_draw_objects_args, textures_args,
        [](const Map::TexDrawObjArgs& obj) {return obj.texture_id;},
        [](const Map::TextureArgs& obj) {return obj.id;}
    ))
        throw std::runtime_error("Textured drawable objects have invalid texture ID(s).");

    if (!validate_id(tex_draw_objects_args, meshes_args,
        [](const Map::TexDrawObjArgs& obj) {return obj.mesh_id;},
        [](const Map::MeshArgs& obj) {return obj.id;}
    ))
        throw std::runtime_error("Textured drawable objects have invalid mesh ID(s).");

    if (!validate_id(unsh_draw_objects_args, meshes_args,
        [](const Map::UnshDrawObjArgs& obj) {return obj.mesh_id;},
        [](const Map::MeshArgs& obj) {return obj.id;}
    ))
        throw std::runtime_error("Unshaded drawable objects have invalid mesh ID(s).");

    if (!validate_id(moving_light_bulbs_args, point_lights_args,
        [](const Map::MovingLightBulbArgs& obj) {return obj.point_light_id;},
        [](const Map::PointLightArgs& obj) {return obj.id;}
    ))
        throw std::runtime_error("Moving light bulbs have invalid point light ID(s).");

    if (!validate_id(moving_light_bulbs_args, drawable_ids,
        [](const Map::MovingLightBulbArgs& obj) {return obj.point_light_id;},
        [](const uint64_t id) {return id;}
    ))
        throw std::runtime_error("Moving light bulbs have invalid point light ID(s).");

    // Check files.
    for (const auto& cur_mesh : meshes_args)
        check_file(MESHES_PATH + cur_mesh.file, ".obj");
    for (const auto& cur_texture : textures_args)
        check_file(TEXTURES_PATH + cur_texture.file, ".ktx");
}

void Map::set_map(RenderingServer& rs, PhysicsServer& ps,
                  std::vector<std::shared_ptr<MovingLightBulb>>& moving_bulbs) {
    // Construct textures.
    std::map<uint64_t, std::shared_ptr<KTXTexture>> textures;
    for (const auto& tex_arg : textures_args)
        textures[tex_arg.id] = std::make_shared<KTXTexture>(TEXTURES_PATH + tex_arg.file);

    // Construct meshes.
    std::map<uint64_t, std::shared_ptr<Mesh>> meshes;
    for (const auto& mesh_arg : meshes_args)
        meshes[mesh_arg.id] = std::make_shared<Mesh>(MESHES_PATH + mesh_arg.file);

    // Construct and emplace textured drawable objects.
    std::map<uint64_t, std::shared_ptr<SpatialObject>> spatial_objects;
    for (const auto& tex_draw_arg : tex_draw_objects_args) {
        const auto cur_obj {std::make_shared<TexturedDrawableObject>(
            textures[tex_draw_arg.texture_id],
            meshes[tex_draw_arg.mesh_id]
        )};
        cur_obj->translation = tex_draw_arg.position;
        cur_obj->scale = tex_draw_arg.scale;
        cur_obj->rotation = tex_draw_arg.rotation;
        rs.add_drawable_object(cur_obj);
        spatial_objects[tex_draw_arg.id] = cur_obj;
    }

    // Construct and emplace unshaded drawable objects.
    for (const auto& unsh_draw_arg : unsh_draw_objects_args) {
        const auto cur_obj {std::make_shared<UnshadedDrawableObject>(
            meshes[unsh_draw_arg.mesh_id],
            unsh_draw_arg.color
        )};
        cur_obj->translation = unsh_draw_arg.position;
        cur_obj->scale = unsh_draw_arg.scale;
        cur_obj->rotation = unsh_draw_arg.rotation;
        rs.add_drawable_object(cur_obj);
        spatial_objects[unsh_draw_arg.id] = cur_obj;
    }

    // Construct and emplace point lights.
    std::map<uint64_t, std::shared_ptr<PointLight>> point_lights;
    for (const auto& light_args : point_lights_args) {
        const auto cur_light {std::make_shared<PointLight>(
            PointLight({
                light_args.position, light_args.color, light_args.diffuse_strength,
                light_args.const_coeff, light_args.linear_coeff, light_args.quadratic_coeff
            })
        )};
        rs.draw_params.point_lights.push_back(cur_light);
        point_lights[light_args.id] = cur_light;
    }

    // Construct and emplace moving light bulbs.
    for (const auto& bulb_args : moving_light_bulbs_args) {
        const auto bulb = std::make_shared<MovingLightBulb>(
            bulb_args.path, bulb_args.speed
        );
        bulb->point_light = point_lights[bulb_args.point_light_id];
        bulb->bulb_obj = spatial_objects[bulb_args.drawable_object_id];
        moving_bulbs.push_back(bulb);
    }

    // Construct and emplace flat floors.
    for (const auto& floor : flat_floors_args) {
        ps.add_flat_floor(std::make_shared<FloorObject>(
            FloorObject({floor.rect, floor.height})
        ));
    }

    // Construct and emplace rectangular walls.
    for (const auto& wall : rect_walls_args) {
        ps.add_rectangular_wall(std::make_shared<RectangularWall>(
            wall.rect
        ));
    }

    // Construct and emplace cuboid objects.
    for (const auto& obj : cuboid_objects_args) {
        ps.add_cuboid_object(std::make_shared<CuboidObject>(
            obj.rect, obj.bottom_y, obj.top_y
        ));
    }

    // Set vertical bounds.
    ps.set_bounds(left_bound, right_bound, back_bound, front_bound);
}
