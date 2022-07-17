#include <fstream>
#include <optional>

#include <nlohmann/json.hpp>

#include "Map.hpp"
#include "KTXTexture.hpp"

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

void from_json(const json& local_json, std::shared_ptr<FloorObject>& floor) {
    floor = std::make_shared<FloorObject>(FloorObject({
        local_json.at("rect").get<Rect>(),
        static_cast<float>(local_json.at("height").get<double>())
    }));
}

void from_json(const json& local_json, std::shared_ptr<Mesh>& mesh) {
    const std::string file {local_json.at("file").get<std::string>()};

    mesh = std::make_shared<Mesh>(MESHES_PATH + file);
}

void from_json(const json& local_json, std::shared_ptr<Texture>& texture) {
    const std::string file {local_json.at("file").get<std::string>()};

    texture = std::make_shared<KTXTexture>(TEXTURES_PATH + file);
}

void from_json(const json& local_json, std::shared_ptr<RectangularWall>& wall) {
    const Rect rect {local_json.at("rect").get<Rect>()};
    wall = std::make_shared<RectangularWall>(rect);
}

void from_json(const json& local_json, std::shared_ptr<CuboidObject>& object) {
    const Rect rect {local_json.at("rect").get<Rect>()};
    const float bottom_y = local_json.at("bottom_y").get<double>();
    const float top_y = local_json.at("top_y").get<double>();

    object = std::make_shared<CuboidObject>(rect, bottom_y, top_y);
}

void from_json(const json& local_json, std::shared_ptr<PointLight>& light) {
    light = std::make_shared<PointLight>();
    light->color = vec3_from_json(local_json.at("color"), true);
    light->position = vec3_from_json(local_json, "position", DEFAULT_POS);
    local_json.at("diffuse_strength").get_to(light->diffuse_strength);
    local_json.at("const_coeff").get_to(light->const_coeff);
    local_json.at("linear_coeff").get_to(light->linear_coeff);
    local_json.at("quadratic_coeff").get_to(light->quadratic_coeff);
}

void from_json(const json& local_json, std::shared_ptr<MovingLightBulb>& bulb) {
    const std::vector<glm::vec3> path = local_json.at("path");
    const float speed = local_json.at("speed").get<double>();

    bulb = std::make_shared<MovingLightBulb>(path, speed);
}

void handle_flat_floors(Map& map, const json& root_json) {
    root_json.at("flat_floors").get_to(map.flat_floors);
}

std::vector<std::pair<uint64_t, std::shared_ptr<Mesh>>>
handle_meshes(const json& root_json) {
    std::vector<std::pair<uint64_t, std::shared_ptr<Mesh>>> result;

    const json meshes_json = root_json.at("meshes");
    for (const auto& element : meshes_json)
        result.push_back({element.at("id").get<uint64_t>(), element});

    return result;
}

std::vector<std::pair<uint64_t, std::shared_ptr<Texture>>>
handle_textures(const json& root_json) {
    std::vector<std::pair<uint64_t, std::shared_ptr<Texture>>> result;

    const json textures_json = root_json.at("textures");
    for (const auto& element : textures_json)
        result.push_back({element.at("id").get<uint64_t>(), element});

    return result;
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

void handle_textured_drawables(Map& map, const json& root_json,
                               const std::vector<std::pair<uint64_t, std::shared_ptr<Mesh>>>& meshes,
                               const std::vector<std::pair<uint64_t, std::shared_ptr<Texture>>>& textures) {
    map.tex_draw_objects.clear();

    const json drawables_json = root_json.at("textured_drawables");
    for (const auto& element : drawables_json) {
        const uint64_t mesh_id {element.at("mesh_id").get<uint64_t>()};
        const uint64_t texture_id {element.at("texture_id").get<uint64_t>()};
        const glm::vec3 position {vec3_from_json(element, "position", DEFAULT_POS)};
        const glm::vec3 scale {vec3_from_json(element, "scale", DEFAULT_SCALE)};
        const glm::vec3 rotation {vec3_from_json(element, "rotation", DEFAULT_ROT)};

        // Get mesh from mesh ID.
        const auto mesh {find_in_vector(meshes, mesh_id)};
        if (!mesh.has_value())
            throw std::runtime_error("Invalid mesh_id.");

        // Get texture from texture ID.
        const auto texture {find_in_vector(textures, texture_id)};
        if (!texture.has_value())
            throw std::runtime_error("Invalid texture_id");

        map.tex_draw_objects.push_back(std::make_shared<TexturedDrawableObject>(
            texture.value(), mesh.value()
        ));
        map.tex_draw_objects.back()->translation = position;
        map.tex_draw_objects.back()->scale = scale;
        map.tex_draw_objects.back()->rotation = glm::quat(rotation);
    }
}

std::vector<std::pair<uint64_t, std::shared_ptr<UnshadedDrawableObject>>>
handle_unshaded_drawables(Map& map, const json& root_json,
                          const std::vector<std::pair<uint64_t, std::shared_ptr<Mesh>>>& meshes) {
    map.unsh_draw_objects.clear();

    std::vector<std::pair<uint64_t, std::shared_ptr<UnshadedDrawableObject>>> result;
    const json drawables_json = root_json.at("unshaded_drawables");
    for (const auto& element : drawables_json) {
        const uint64_t id {element.at("id").get<uint64_t>()};
        const uint64_t mesh_id {element.at("mesh_id").get<uint64_t>()};
        const glm::vec3 color {vec3_from_json(element.at("color"), true)};
        const glm::vec3 position {vec3_from_json(element, "position", DEFAULT_POS)};
        const glm::vec3 scale {vec3_from_json(element, "scale", DEFAULT_SCALE)};
        const glm::vec3 rotation {vec3_from_json(element, "rotation", DEFAULT_ROT)};

        // Get mesh from mesh ID.
        const auto mesh {find_in_vector(meshes, mesh_id)};
        if (!mesh.has_value())
            throw std::runtime_error("Invalid mesh_id.");

        map.unsh_draw_objects.push_back(std::make_shared<UnshadedDrawableObject>(
            mesh.value(), color
        ));
        map.unsh_draw_objects.back()->translation = position;
        map.unsh_draw_objects.back()->scale = scale;
        map.unsh_draw_objects.back()->rotation = glm::quat(rotation);
        result.push_back({id, map.unsh_draw_objects.back()});
    }

    return result;
}

void handle_rectangular_walls(Map& map, const json& root_json) {
    root_json.at("rectangular_walls").get_to(map.rect_walls);
}

void handle_cuboid_objects(Map& map, const json& root_json) {
    root_json.at("cuboid_objects").get_to(map.cuboid_objects);
}

std::vector<std::pair<uint64_t, std::shared_ptr<PointLight>>>
handle_point_lights(Map& map, const json& root_json) {
    map.point_lights.clear();

    std::vector<std::pair<uint64_t, std::shared_ptr<PointLight>>> result;
    const json lights_json = root_json.at("point_lights");
    for (const auto& element : lights_json) {
        const uint64_t id {element.at("id").get<uint64_t>()};
        const std::shared_ptr<PointLight> light {element};
        map.point_lights.push_back(light);
        result.push_back({id, light});
    }

    return result;
}

void handle_moving_light_bulbs(Map& map, const json& root_json,
        const std::vector<std::pair<uint64_t, std::shared_ptr<PointLight>>>& point_lights,
        const std::vector<std::pair<uint64_t, std::shared_ptr<UnshadedDrawableObject>>>& unsh_draw_objects) {
    map.moving_light_bulbs.clear();

    const json bulbs_json = root_json.at("moving_light_bulbs");
    for (const auto& element : bulbs_json) {
        const uint64_t point_light_id {element.at("point_light_id").get<uint64_t>()};
        const uint64_t drawable_object_id {element.at("drawable_object_id").get<uint64_t>()};
        const std::shared_ptr<MovingLightBulb>& bulb = element;

        // Get point light from its ID.
        const auto light {find_in_vector(point_lights, point_light_id)};
        if (!light.has_value())
            throw std::runtime_error("Invalid point_light_id.");

        // Get drawable object from its ID.
        const auto draw_obj {find_in_vector(unsh_draw_objects, drawable_object_id)};
        if (!draw_obj.has_value())
            throw std::runtime_error("Invalid drawable_object_id.");

        bulb->point_light = light.value();
        bulb->drawable_obj = draw_obj.value();
        map.moving_light_bulbs.push_back(bulb);
    }
}

void Map::from_json(std::string_view file_path) {
    using std::literals::operator""s;

    json json_obj;
#ifdef NDEBUG
    try {
#endif
        std::ifstream stream(file_path.data());
        stream >> json_obj;

        handle_root_elements(*this, json_obj);
        handle_flat_floors(*this, json_obj);
        auto meshes = handle_meshes(json_obj);
        auto textures = handle_textures(json_obj);
        handle_textured_drawables(*this, json_obj, meshes, textures);
        auto unsh_draw_objects = handle_unshaded_drawables(*this, json_obj, meshes);
        handle_rectangular_walls(*this, json_obj);
        handle_cuboid_objects(*this, json_obj);
        auto point_lights {handle_point_lights(*this, json_obj)};
        handle_moving_light_bulbs(*this, json_obj, point_lights, unsh_draw_objects);
#ifdef NDEBUG
    }
    catch (const json::parse_error& err) {
        std::stringstream message;
        message << "JSON parsing error in file \"" << file_path <<
                "\" on byte " << err.byte <<
                ": " << err.what();
        throw std::runtime_error(message.str());
    }
    catch (const json::type_error& err) {
        std::stringstream message;
        message << "JSON type error in file \"" << file_path <<
                "\": " << err.what();
        throw std::runtime_error(message.str());
    }
    catch (const json::out_of_range& err) {
        std::stringstream message;
        message << "JSON doesn't contain required data in file \"" << file_path <<
                "\": " << err.what();
        throw std::runtime_error(message.str());
    }
#endif
}
