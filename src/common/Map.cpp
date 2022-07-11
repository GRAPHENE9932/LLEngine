#include <fstream>
#include <unordered_map>

#include <toml++/toml.h>

#include "Map.hpp"
#include "KTXTexture.hpp"

Map::Map(const std::string& toml_file_path) {
    from_toml(toml_file_path);
}

constexpr const char* MESHES_PATH = "res/meshes/";
constexpr const char* TEXTURES_PATH = "res/textures/";

constexpr glm::vec3 DEFAULT_POS {0.0f, 0.0f, 0.0f};
constexpr glm::vec3 DEFAULT_SCALE {1.0f, 1.0f, 1.0f};
constexpr glm::vec3 DEFAULT_ROT {0.0f, 0.0f, 0.0f};

void map_parse_error(const std::string& message, const std::string& file_path) {
    throw std::runtime_error(
        "Failed to parse the map file: " +
        message + "\nFile path: " + file_path
    );
}

void invalid_toml_entry_error(const std::string entry_name, const std::string& file_path) {
    map_parse_error(
        "invalid/non-existent \"" + entry_name + "\" entry",
        file_path
    );
}

void handle_root_elements(Map& map, const toml::table& root_table, const std::string& file_path) {
    const toml::value<int64_t>* version {root_table.get_as<int64_t>("version")};
    if (!version)
        invalid_toml_entry_error("version", file_path);
    if (version->get() != 1)
        map_parse_error("Invalid version.", file_path);

    const toml::value<std::string>* name {root_table.get_as<std::string>("name")};
    if (!name)
        invalid_toml_entry_error("name", file_path);
    map.name = name->get();

    const toml::value<double>* left_bound {root_table.get_as<double>("left_bound")};
    if (!left_bound)
        invalid_toml_entry_error("left_bound", file_path);
    map.left_bound = left_bound->get();

    const toml::value<double>* right_bound {root_table.get_as<double>("right_bound")};
    if (!right_bound)
        invalid_toml_entry_error("right_bound", file_path);
    map.right_bound = right_bound->get();

    const toml::value<double>* back_bound {root_table.get_as<double>("back_bound")};
    if (!back_bound)
        invalid_toml_entry_error("back_bound", file_path);
    map.back_bound = back_bound->get();

    const toml::value<double>* front_bound {root_table.get_as<double>("front_bound")};
    if (!front_bound)
        invalid_toml_entry_error("front_bound", file_path);
    map.front_bound = front_bound->get();
}

std::optional<Rect> table_to_rect(const toml::table* rect_table) {
    if (!rect_table)
        return {};

    const toml::value<double>* pos_x {rect_table->get_as<double>("pos_x")};
    const toml::value<double>* pos_z {rect_table->get_as<double>("pos_z")};
    const toml::value<double>* size_x {rect_table->get_as<double>("size_x")};
    const toml::value<double>* size_z {rect_table->get_as<double>("size_z")};

    if (!(pos_x && pos_z && size_x && size_z))
        return {};

    return std::make_optional<Rect>({{pos_x->get(), pos_z->get()}, {size_x->get(), size_z->get()}});
}

void handle_flat_floors(Map& map, const toml::table& root_table, const std::string& file_path) {
    map.flat_floors.clear();

    const toml::array* flat_floors {root_table.get_as<toml::array>("flat_floors")};

    if (!flat_floors)
        return;

    flat_floors->for_each([&map, &file_path](auto&& element) {
        const toml::table* flat_floor {element.as_table()};
        if (!flat_floor)
            invalid_toml_entry_error("flat_floors", file_path);

        // rect (table).
        const toml::table* rect_table {flat_floor->get_as<toml::table>("rect")};
        std::optional<Rect> rect {table_to_rect(rect_table)};
        if (!rect.has_value())
            invalid_toml_entry_error("flat_floors.rect", file_path);

        // height (float).
        const toml::value<double>* height {flat_floor->get_as<double>("height")};
        if (!height)
            invalid_toml_entry_error("flat_floors.height", file_path);

        map.flat_floors.push_back(std::make_shared<FloorObject>(
            FloorObject({rect.value(), static_cast<float>(height->get())})
        ));
    });
}

std::vector<std::pair<int64_t, std::shared_ptr<Mesh>>>
handle_meshes(const toml::table& root_table, const std::string& file_path) {
    const toml::array* meshes {root_table.get_as<toml::array>("meshes")};
    if (!meshes)
        return {};

    std::vector<std::pair<int64_t, std::shared_ptr<Mesh>>> result;
    meshes->for_each([&result, &file_path](auto&& element) {
        const toml::table* mesh_table {element.as_table()};
        if (!mesh_table)
            invalid_toml_entry_error("meshes", file_path);

        // id (int).
        const toml::value<int64_t>* id {mesh_table->get_as<int64_t>("id")};
        if (!id)
            invalid_toml_entry_error("meshes.id", file_path);

        // file (string)
        const toml::value<std::string>* file {mesh_table->get_as<std::string>("file")};
        if (!file)
            invalid_toml_entry_error("meshes.file", file_path);

        result.push_back({id->get(), std::make_shared<Mesh>(MESHES_PATH + file->get())});
    });

    return result;
}

std::vector<std::pair<int64_t, std::shared_ptr<Texture>>>
handle_textures(const toml::table& root_table, const std::string& file_path) {
    const toml::array* textures {root_table.get_as<toml::array>("textures")};
    if (!textures)
        return {};

    std::vector<std::pair<int64_t, std::shared_ptr<Texture>>> result;
    textures->for_each([&result, &file_path](auto&& element) {
        const toml::table* texture_table {element.as_table()};
        if (!texture_table)
            invalid_toml_entry_error("textures", file_path);

        // id (int).
        const toml::value<int64_t>* id {texture_table->get_as<int64_t>("id")};
        if (!id)
            invalid_toml_entry_error("textures.id", file_path);

        // file (string).
        const toml::value<std::string>* file {texture_table->get_as<std::string>("file")};
        if (!file)
            invalid_toml_entry_error("textures.file", file_path);

        result.push_back({id->get(), std::make_shared<KTXTexture>(TEXTURES_PATH + file->get())});
    });

    return result;
}

std::optional<glm::vec3> table_to_vec3(const toml::table* vec3_table, const bool is_color = false) {
    if (!vec3_table)
        return {};

    const toml::value<double>* x {vec3_table->get_as<double>(is_color ? "r" : "x")};
    const toml::value<double>* y {vec3_table->get_as<double>(is_color ? "g" : "y")};
    const toml::value<double>* z {vec3_table->get_as<double>(is_color ? "b" : "z")};

    if (!(x && y && z))
        return {};

    return std::make_optional<glm::vec3>({x->get(), y->get(), z->get()});
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

void handle_textured_drawables(Map& map, const toml::table& root_table, const std::string& file_path,
                               const std::vector<std::pair<int64_t, std::shared_ptr<Mesh>>>& meshes,
                               const std::vector<std::pair<int64_t, std::shared_ptr<Texture>>>& textures) {
    map.tex_draw_objects.clear();

    const toml::array* drawables {root_table.get_as<toml::array>("textured_drawables")};
    if (!drawables)
        return;

    drawables->for_each([&map, &file_path, &meshes, &textures](auto&& element) {
        const toml::table* drawable_table {element.as_table()};
        if (drawable_table == nullptr)
            invalid_toml_entry_error("textured_drawables", file_path);

        // mesh_id (int).
        const toml::value<int64_t>* mesh_id {drawable_table->get_as<int64_t>("mesh_id")};
        if (!mesh_id)
            invalid_toml_entry_error("textured_drawables.mesh_id", file_path);

        // texture_id (int).
        const toml::value<int64_t>* texture_id {drawable_table->get_as<int64_t>("texture_id")};
        if (!texture_id)
            invalid_toml_entry_error("textured_drawables.texture_id", file_path);

        // position (table {x, y, z}).
        const toml::table* pos_table {drawable_table->get_as<toml::table>("position")};
        const glm::vec3 position {table_to_vec3(pos_table).value_or(DEFAULT_POS)};

        // scale (table {x, y, z}).
        const toml::table* scale_table {drawable_table->get_as<toml::table>("scale")};
        const glm::vec3 scale {table_to_vec3(scale_table).value_or(DEFAULT_SCALE)};

        // rotation (table {x, y, z}).
        const toml::table* rot_table {drawable_table->get_as<toml::table>("rotation")};
        const glm::vec3 rotation {table_to_vec3(rot_table).value_or(DEFAULT_ROT)};

        // Get mesh from ID.
        const std::optional<std::shared_ptr<Mesh>>& mesh {
            find_in_vector(meshes, mesh_id->get())
        };
        if (!mesh.has_value())
            map_parse_error("Invalid mesh_id in one of the textured drawable objects", file_path);

        // Get texture from ID.
        const std::optional<std::shared_ptr<Texture>>& texture {
            find_in_vector(textures, texture_id->get())
        };
        if (!texture.has_value())
            map_parse_error("Invalid texture_id in one of the textured drawable objects", file_path);

        map.tex_draw_objects.push_back(std::make_shared<TexturedDrawableObject>(
            texture.value(), mesh.value()
        ));
        map.tex_draw_objects.back()->translation = position;
        map.tex_draw_objects.back()->scale = scale;
        map.tex_draw_objects.back()->rotation = rotation;
    });
}

std::vector<std::pair<int64_t, std::shared_ptr<UnshadedDrawableObject>>>
handle_unshaded_drawables(Map& map, const toml::table& root_table, const std::string& file_path,
                          const std::vector<std::pair<int64_t, std::shared_ptr<Mesh>>>& meshes) {
    map.unsh_draw_objects.clear();

    const toml::array* drawables {root_table.get_as<toml::array>("unshaded_drawables")};
    if (!drawables)
        return {};

    std::vector<std::pair<int64_t, std::shared_ptr<UnshadedDrawableObject>>> result;
    drawables->for_each([&map, &meshes, &result, &file_path](auto&& element) {
        const toml::table* drawable_table {element.as_table()};
        if (!drawable_table)
            invalid_toml_entry_error("unshaded_drawables", file_path);

        // id (int).
        const toml::value<int64_t>* id {drawable_table->get_as<int64_t>("id")};
        if (!id)
            invalid_toml_entry_error("unshaded_drawables.id", file_path);

        // mesh_id (int).
        const toml::value<int64_t>* mesh_id {drawable_table->get_as<int64_t>("mesh_id")};
        if (!mesh_id)
            invalid_toml_entry_error("unshaded_drawables.mesh_id", file_path);

        // color (table {r, g, b}).
        const toml::table* color_table {drawable_table->get_as<toml::table>("color")};
        const std::optional<glm::vec3> color {table_to_vec3(color_table, true)};
        if (!color.has_value())
            invalid_toml_entry_error("unshaded_drawables.color", file_path);

        // position (table {x, y, z}).
        const toml::table* pos_table {drawable_table->get_as<toml::table>("position")};
        const glm::vec3 position {table_to_vec3(pos_table).value_or(DEFAULT_POS)};

        // scale (table {x, y, z}).
        const toml::table* scale_table {drawable_table->get_as<toml::table>("scale")};
        const glm::vec3 scale {table_to_vec3(scale_table).value_or(DEFAULT_SCALE)};

        // rotation (table {x, y, z}).
        const toml::table* rot_table {drawable_table->get_as<toml::table>("rotation")};
        const glm::vec3 rotation {table_to_vec3(rot_table).value_or(DEFAULT_ROT)};

        // Get mesh from ID.
        const std::optional<std::shared_ptr<Mesh>>& mesh {
            find_in_vector(meshes, mesh_id->get())
        };
        if (!mesh.has_value())
            map_parse_error("Invalid mesh_id in one of the unshaded drawable objects", file_path);

        map.unsh_draw_objects.push_back(std::make_shared<UnshadedDrawableObject>(
            mesh.value(), color.value()
        ));
        map.unsh_draw_objects.back()->translation = position;
        map.unsh_draw_objects.back()->scale = scale;
        map.unsh_draw_objects.back()->rotation = rotation;
        result.push_back({id->get(), map.unsh_draw_objects.back()});
    });

    return result;
}

void handle_rectangular_walls(Map& map, const toml::table& root_table, const std::string& file_path) {
    map.rect_walls.clear();

    const toml::array* walls {root_table.get_as<toml::array>("rectangular_walls")};
    if (!walls)
        return;

    walls->for_each([&map, &file_path](auto&& element) {
        const toml::table* wall_table {element.as_table()};
        if (!wall_table)
            invalid_toml_entry_error("rectangular_walls", file_path);

        // rect.
        const toml::table* rect_table {wall_table->get_as<toml::table>("rect")};
        const std::optional<Rect> rect {table_to_rect(rect_table)};
        if (!rect.has_value())
            invalid_toml_entry_error("rectangular_walls.rect", file_path);

        map.rect_walls.push_back(std::make_shared<RectangularWall>(
            rect.value()
        ));
    });
}

void handle_cuboid_objects(Map& map, const toml::table& root_table, const std::string& file_path) {
    map.cuboid_objects.clear();

    const toml::array* objects {root_table.get_as<toml::array>("cuboid_objects")};
    if (!objects)
        return;

    objects ->for_each([&map, &file_path](auto&& element) {
        const toml::table* object_table {element.as_table()};
        if (!object_table)
            invalid_toml_entry_error("cuboid_objects", file_path);

        // rect.
        const toml::table* rect_table {object_table->get_as<toml::table>("rect")};
        std::optional<Rect> rect {table_to_rect(rect_table)};
        if (!rect.has_value())
            invalid_toml_entry_error("cuboid_objects.rect", file_path);

        // bottom_y.
        const toml::value<double>* bottom_y {object_table->get_as<double>("bottom_y")};
        if (!bottom_y)
            invalid_toml_entry_error("cuboid_objects.bottom_y", file_path);

        // top_y.
        const toml::value<double>* top_y {object_table->get_as<double>("top_y")};
        if (!top_y)
            invalid_toml_entry_error("cuboid_objects.top_y", file_path);

        map.cuboid_objects.push_back(std::make_shared<CuboidObject>(
            rect.value(), bottom_y->get(), top_y->get()
        ));
    });
}

std::vector<std::pair<int64_t, std::shared_ptr<PointLight>>>
handle_point_lights(Map& map, const toml::table& root_table, const std::string& file_path) {
    map.point_lights.clear();

    const toml::array* point_lights {root_table["point_lights"].as_array()};
    if (!point_lights)
        return {};

    std::vector<std::pair<int64_t, std::shared_ptr<PointLight>>> result;
    point_lights->for_each([&map, &file_path, &result](auto&& element) {
        const toml::table* light_table {element.as_table()};
        if (!light_table)
            invalid_toml_entry_error("point_lights", file_path);

        // id.
        const toml::value<int64_t>* id {light_table->get_as<int64_t>("id")};
        if (!id)
            invalid_toml_entry_error("point_lights.id", file_path);

        // color.
        const toml::table* color_table {light_table->get_as<toml::table>("color")};
        const std::optional<glm::vec3> color {table_to_vec3(color_table, true)};
        if (!color.has_value())
            invalid_toml_entry_error("point_lights.color", file_path);

        // position.
        const toml::table* pos_table {light_table->get_as<toml::table>("position")};
        const glm::vec3 pos {table_to_vec3(pos_table).value_or(DEFAULT_POS)};

        // diffuse_strength.
        const toml::value<double>* diffuse_strength {light_table->get_as<double>("diffuse_strength")};
        if (!diffuse_strength)
            invalid_toml_entry_error("point_lights.diffuse_strength", file_path);

        // const_coeff.
        const toml::value<double>* const_coeff {light_table->get_as<double>("const_coeff")};
        if (!const_coeff)
            invalid_toml_entry_error("point_lights.const_coeff", file_path);

        // linear_coeff.
        const toml::value<double>* linear_coeff {light_table->get_as<double>("linear_coeff")};
        if (!linear_coeff)
            invalid_toml_entry_error("point_lights.linear_coeff", file_path);

        // quadratic_coeff.
        const toml::value<double>* quadratic_coeff {light_table->get_as<double>("quadratic_coeff")};
        if (!quadratic_coeff)
            invalid_toml_entry_error("point_lights.quadratic_coeff", file_path);

        map.point_lights.push_back(std::make_shared<PointLight>(
            pos, color.value(), diffuse_strength->get(), const_coeff->get(),
            linear_coeff->get(), quadratic_coeff->get()
        ));
        result.push_back({id->get(), map.point_lights.back()});
    });

    return result;
}

void handle_moving_light_bulbs(Map& map, const toml::table& root_table, const std::string& file_path,
        const std::vector<std::pair<int64_t, std::shared_ptr<PointLight>>>& point_lights,
        const std::vector<std::pair<int64_t, std::shared_ptr<UnshadedDrawableObject>>>& unsh_draw_objects) {
    map.moving_light_bulbs.clear();

    const toml::array* moving_bulbs {root_table.get_as<toml::array>("moving_light_bulbs")};
    if (!moving_bulbs)
        return;

    moving_bulbs->for_each(
        [&map, &file_path, &point_lights, &unsh_draw_objects](auto&& element) {
        const toml::table* bulb_table {element.as_table()};
        if (!bulb_table)
            invalid_toml_entry_error("moving_light_bulbs", file_path);

        // point_light_id.
        const toml::value<int64_t>* point_light_id {bulb_table->get_as<int64_t>("point_light_id")};
        if (!point_light_id)
            invalid_toml_entry_error("moving_light_bulbs.point_light_id", file_path);

        // drawable_object_id.
        const toml::value<int64_t>* drawable_object_id {bulb_table->get_as<int64_t>("drawable_object_id")};
        if (!drawable_object_id)
            invalid_toml_entry_error("moving_light_bulbs.drawable_object_id", file_path);

        // speed.
        const toml::value<double>* speed {bulb_table->get_as<double>("speed")};
        if (!speed)
            invalid_toml_entry_error("moving_light_bulbs.speed", file_path);

        // path.
        std::vector<glm::vec3> path;
        const toml::array* path_array {bulb_table->get_as<toml::array>("path")};
        if (path_array) {
            path_array->for_each([&path, &file_path](auto&& element) {
                const toml::table* point_table {element.as_table()};
                std::optional<glm::vec3> point {table_to_vec3(point_table)};
                if (!point.has_value())
                    invalid_toml_entry_error("moving_light_bulbs.path", file_path);

                path.push_back(point.value());
            });
        }
        if (path.size() < 2)
            invalid_toml_entry_error("moving_light_bulbs.path", file_path);

        // Get point light from it's ID.
        const auto point_light {find_in_vector(point_lights, point_light_id->get())};
        if (!point_light.has_value())
            map_parse_error("Invalid point_light_id in one of the moving light bulbs.", file_path);

        // Get unshaded drawable object from it's ID.
        const auto unsh_draw_object {find_in_vector(unsh_draw_objects, drawable_object_id->get())};
        if (!unsh_draw_object.has_value())
            map_parse_error("Invalid drawable_object_id in one of the moving light bulbs.", file_path);

        map.moving_light_bulbs.push_back(std::make_shared<MovingLightBulb>(
            path, speed->get()
        ));
        map.moving_light_bulbs.back()->drawable_obj = unsh_draw_object.value();
        map.moving_light_bulbs.back()->point_light = point_light.value();
    });
}

void Map::from_toml(const std::string& file_path) {
    toml::table root_table;
    try {
        root_table = toml::parse_file(file_path);
    }
    catch (const toml::parse_error& err) {
        map_parse_error(
            std::string("toml parsing error: ") + err.what(),
            file_path
        );
    }
    handle_root_elements(*this, root_table, file_path);
    handle_flat_floors(*this, root_table, file_path);
    auto meshes = handle_meshes(root_table, file_path);
    auto textures = handle_textures(root_table, file_path);
    handle_textured_drawables(*this, root_table, file_path, meshes, textures);
    auto unsh_draw_objects = handle_unshaded_drawables(*this, root_table, file_path, meshes);
    handle_rectangular_walls(*this, root_table, file_path);
    handle_cuboid_objects(*this, root_table, file_path);
    auto point_lights {handle_point_lights(*this, root_table, file_path)};
    handle_moving_light_bulbs(*this, root_table, file_path, point_lights, unsh_draw_objects);
}
