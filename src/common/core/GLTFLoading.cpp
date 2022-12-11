#include <cstdint>
#include <fstream>
#include <charconv>
#include <unordered_set>

#include <glm/vec3.hpp>
#include <glm/gtx/quaternion.hpp>
#include <nlohmann/json.hpp>

#include "GLTF.hpp"
#include "utils/json_conversion.hpp"

using json = nlohmann::json;

constexpr uint32_t GLTF_MAGIC = 0x46546C67;
constexpr int ASSET_VERSION_MAJOR = 2;
constexpr int ASSET_VERSION_MINOR = 0;
constexpr uint32_t GLB_VERSION = 2;
constexpr uint32_t CHUNK_TYPE_JSON = 0x4E4F534A;
constexpr uint32_t CHUNK_TYPE_BIN = 0x004E4942;

constexpr float DEFAULT_ROUGHNESS = 1.0f;
constexpr float DEFAULT_METALLIC = 1.0f;

constexpr glm::vec3 DEFAULT_TRANSLATION = {0.0f, 0.0f, 0.0f};
constexpr glm::vec3 DEFAULT_SCALE = {1.0f, 1.0f, 1.0f};
constexpr glm::quat DEFAULT_ROTATION = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

constexpr std::string_view TEXTURES_LOCATION = "res/textures";

constexpr std::array<std::string_view, 1> SUPPORTED_EXTENSIONS {
    "KHR_texture_transform"
};

struct Header {
    uint32_t magic;
    uint32_t version;
    uint32_t length;
};

/// Returns true in case of success.
bool check_asset_version(std::string_view version) {
    const size_t point_pos = version.find('.');
    if (point_pos == std::string_view::npos)
        return false;

    int minor;
    const auto minor_res =
        std::from_chars(version.data() + point_pos + 1, version.data() + version.size(), minor);
    if (minor_res.ec == std::errc::invalid_argument)
        return false;

    int major;
    const auto major_res =
        std::from_chars(version.data(), version.data() + point_pos, major);
    if (major_res.ec == std::errc::invalid_argument)
        return false;

    return major == ASSET_VERSION_MAJOR && minor == ASSET_VERSION_MINOR;
}

inline std::string append_paths(std::string_view path_1,
                                std::string_view path_2) {
    return std::filesystem::path(path_1) / std::filesystem::path(path_2);
}

void construct_texture_params(GLTF& gltf, const json& gltf_json,
        std::string_view gltf_path, std::streamsize bin_buffer_offset) {
    gltf.textures.clear();

    if (!gltf_json.contains("textures"))
        return;
    
    for (const json& cur_json : gltf_json.at("textures")) {
        Texture::Parameters result;

        if (cur_json.contains("sampler")) {
            const json& cur_sampler = gltf_json.at("samplers").at(cur_json["sampler"].get<uint32_t>());
            result.magnification_filter = get_optional(cur_sampler, "magFilter", GL_LINEAR);
            result.minification_filter = get_optional(cur_sampler, "minFilter", GL_LINEAR);
            result.wrap_s = get_optional(cur_sampler, "wrapS", GL_REPEAT);
            result.wrap_t = get_optional(cur_sampler, "wrapT", GL_REPEAT);
        }
        else {
            result.magnification_filter = GL_LINEAR;
            result.minification_filter = GL_LINEAR;
            result.wrap_s = GL_REPEAT;
            result.wrap_t = GL_REPEAT;
        }

        // TODO: Placeholder image.
        if (!cur_json.contains("source"))
            throw std::runtime_error("glTF texture doesn't have the source.");
        
        const json& image_json = gltf_json.at("images").at(cur_json["source"].get<uint32_t>());
        if (image_json.contains("uri")) {
            if (image_json["uri"].get<std::string>().starts_with("data:"))
                throw std::runtime_error("base64 data is not supported here.");
            
            result.file_path = append_paths(
                TEXTURES_LOCATION,
                image_json["uri"].get<std::string>()
            );
            result.offset = 0;
        }
        else if (image_json.contains("bufferView")) {
            const json& buf_view_json = gltf_json.at("bufferViews")[image_json["bufferView"].get<uint32_t>()];

            if (buf_view_json.contains("byteStride"))
                throw std::runtime_error("byteStride property must not be defined for images.");
            
            result.offset = bin_buffer_offset +
                    get_optional<std::streamsize>(buf_view_json, "byteOffset", 0);
            result.file_path = gltf_path;
        }
        else {
            throw std::runtime_error("glTF image doesn't have URI or buffer view.");
        }

        gltf.textures.push_back(result);
    }
}

BasicMaterial<uint32_t>::TextureInfo
handle_texture_info(const json& tex_info_json) {
    BasicMaterial<uint32_t>::TextureInfo result;
    uint32_t tex_coord;

    if (tex_info_json.contains("extensions") &&
        tex_info_json["extensions"].contains("KHR_texture_transform")) {
        const json& khr_tex_trans = tex_info_json["extensions"]["KHR_texture_transform"];

        // Handle texCoord.
        if (khr_tex_trans.contains("texCoord"))
            tex_coord = khr_tex_trans["texCoord"].get<uint32_t>();
        else
            tex_coord = get_optional<uint32_t>(tex_info_json, "texCoord", 0);

        // Handle offset and scale.
        result.uv_offset = get_optional<glm::vec2>(khr_tex_trans, "offset", {0.0f, 0.0f});
        result.uv_scale = get_optional<glm::vec2>(khr_tex_trans, "scale", {1.0f, 1.0f});

        // Handle rotation.
        if (get_optional<float>(khr_tex_trans, "rotation", 0.0f) != 0.0f)
            throw std::runtime_error("Texture coordinate rotation is not supported.");
    }
    else {
        // Handle texCoord.
        tex_coord = get_optional<uint32_t>(tex_info_json, "texCoord", 0);

        // Handle offset, scale and rotation.
        result.uv_offset = {0.0f, 0.0f};
        result.uv_scale = {1.0f, 1.0f};
    }

    // Handle index.
    result.texture = tex_info_json.at("index");

    if (tex_coord != 0)
        throw std::runtime_error("Multiple texture coordinates are not supported.");

    return result;
}

BasicMaterial<uint32_t>::NormalMap
handle_normal_texture_info(const json& tex_info_json) {
    return {
        handle_texture_info(tex_info_json),
        get_optional<float>(tex_info_json, "scale", 1.0f)
    };
}

void construct_material_params(GLTF& gltf, const json& gltf_json) {
    if (!gltf_json.contains("materials"))
        return;
    
    gltf.materials.reserve(gltf_json["materials"].size());
    for (const json& cur_json : gltf_json["materials"]) {
        BasicMaterial<uint32_t> result {};

        // Handle PBR metallic roughness.
        if (cur_json.contains("pbrMetallicRoughness")) {
            const json& pbrmr = cur_json["pbrMetallicRoughness"];

            result.base_color.factor = get_optional<glm::vec4>(
                pbrmr, "baseColorFactor", {1.0f, 1.0f, 1.0f, 1.0f}
            );
            result.metallic_roughness.metallic_factor = get_optional<float>(
                pbrmr, "metallicFactor", DEFAULT_METALLIC
            );
            result.metallic_roughness.roughness_factor = get_optional<float>(
                pbrmr, "roughnessFactor", DEFAULT_ROUGHNESS
            );

            if (pbrmr.contains("metallicRoughnessTexture"))
                result.metallic_roughness.texture = handle_texture_info(
                    pbrmr["metallicRoughnessTexture"]
                );
            else
                result.metallic_roughness.texture = std::nullopt;

            if (pbrmr.contains("baseColorTexture"))
                result.base_color.texture = handle_texture_info(
                    pbrmr["baseColorTexture"]
                );
            else
                result.base_color.texture = std::nullopt;
        }

        // Handle normal map info.
        if (cur_json.contains("normalTexture"))
            result.normal_map = handle_normal_texture_info(cur_json["normalTexture"]);
        else
            result.normal_map = std::nullopt;

        gltf.materials.push_back(result);
    }
}

template<typename T>
struct is_glm_vec : std::false_type {};

template<glm::length_t L, typename T>
struct is_glm_vec<glm::vec<L, T>> : std::true_type {};

template<typename T>
struct is_glm_mat : std::false_type {};

template<glm::length_t C, glm::length_t R, typename T>
struct is_glm_mat<glm::mat<C, R, T>> : std::true_type {};

template<typename T>
constexpr GLenum get_component_type() {
    if constexpr (std::is_same<T, int8_t>())
        return GL_BYTE;
    else if constexpr (std::is_same<T, uint8_t>())
        return GL_UNSIGNED_BYTE;
    else if constexpr (std::is_same<T, int16_t>())
        return GL_SHORT;
    else if constexpr (std::is_same<T, uint16_t>())
        return GL_UNSIGNED_SHORT;
    else if constexpr (std::is_same<T, uint32_t>())
        return GL_UNSIGNED_INT;
    else if constexpr (std::is_same<T, float>())
        return GL_FLOAT;
    else if constexpr (is_glm_vec<T>())
        return get_component_type<typename T::value_type>();
    else
        static_assert("Unknown component type.");
}

template<typename T>
constexpr std::string_view get_accessor_type() {
    if constexpr (std::is_arithmetic<T>()) {
        return "SCALAR";
    }
    else if constexpr (is_glm_vec<T>()) {
        if constexpr (T::length() == 2)
            return "VEC2";
        else if constexpr (T::length() == 3)
            return "VEC3";
        else if constexpr (T::length() == 4)
            return "VEC4";
        else
            static_assert("Invalid vector length");
    }
    else if constexpr (is_glm_mat<T>()) {
        if constexpr (T::length() == 2)
            return "MAT2";
        else if constexpr (T::length() == 3)
            return "MAT3";
        else if constexpr (T::length() == 4)
            return "MAT4";
        else
            static_assert("Invalid matrix length");
    }
    else {
        static_assert("The provided type is neither scalar, vector or matrix.");
    }
}

struct CommonBufferArgs {
    const json& gltf_json;
    std::string_view gltf_path;
    std::streamsize bin_chunk_offset;
    std::map<std::string_view, std::ifstream>& stream_pool;
};

template<typename T>
std::vector<T> read_from_fine_buffer_view(const CommonBufferArgs& args,
        const uint32_t buf_view_index, const std::streamsize offset) {
    const json& buf_view_json = args.gltf_json.at("bufferViews").at(buf_view_index);
    const json& buf_json = args.gltf_json.at("buffers").at(buf_view_json.at("buffer").get<uint32_t>());
    std::string_view uri = buf_json.contains("uri") ?
            buf_json["uri"].get<std::string_view>() :
            args.gltf_path;
    
    // TODO: Support base64 data.
    if (uri.starts_with("data:"))
        throw std::runtime_error("base64 data is not supported here.");

    // Prepare ifstream.
    std::ifstream& stream = args.stream_pool[uri];
    if (!stream.is_open()) {
        stream.exceptions(std::ifstream::failbit);
        stream.open(std::string(uri), std::ios::in | std::ios::binary);
    }
    stream.seekg(
        get_optional<uint64_t>(buf_view_json, "byteOffset", 0) +
        args.bin_chunk_offset +
        offset
    );

    const uint64_t count = buf_view_json.at("byteLength").get<uint64_t>() / sizeof(T);

    std::vector<T> result;
    result.resize(count);

    if (buf_view_json.contains("byteStride")) {
        const uint64_t byte_stride = buf_view_json["byteStride"];
        for (uint64_t i = 0; i < count; i++) {
            stream.read(reinterpret_cast<char*>(&result[i]), sizeof(T));

            if (i < count - 1)
                stream.seekg(byte_stride - sizeof(T), std::ios_base::cur);
        }
    }
    else {
        stream.read(reinterpret_cast<char*>(result.data()), count * sizeof(T));
    }
    return result;
}

template<typename IDX_T, typename VAL_T>
std::vector<VAL_T> read_from_sparse_buffer_view(const CommonBufferArgs& args,
        const json& sparse_json) {
    const json& indices_json = sparse_json.at("indices");
    const json& values_json = sparse_json.at("values");
    
    assert(
        (indices_json.at("componentType").get<GLenum>() == GL_UNSIGNED_BYTE &&
        std::is_same<IDX_T, uint8_t>()) ||
        (indices_json.at("componentType").get<GLenum>() == GL_UNSIGNED_SHORT &&
        std::is_same<IDX_T, uint16_t>()) ||
        (indices_json.at("componentType").get<GLenum>() == GL_UNSIGNED_INT &&
        std::is_same<IDX_T, uint32_t>())
    );

    std::vector<IDX_T> indices = read_from_fine_buffer_view<IDX_T>(
        args, indices_json.at("bufferView").get<uint32_t>(),
        get_optional<std::streamsize>(indices_json, "byteOffset", 0)
    );
    std::vector<VAL_T> values = read_from_fine_buffer_view<VAL_T>(
        args, values_json.at("bufferView").get<uint32_t>(),
        get_optional<std::streamsize>(values_json, "byteOffset", 0)
    );

    std::vector<VAL_T> result;
    result.reserve(indices.size());
    for (const IDX_T cur_index : indices)
        result.push_back(values.at(cur_index));
    
    return result;
}

template<typename T>
std::vector<T> read_from_accessor(const CommonBufferArgs& args, const json& accessor_json) {
    // Check type and componentType.
    if (accessor_json.at("type") != get_accessor_type<T>())
        throw std::runtime_error("Unexpected accessor type.");
    if (accessor_json.at("componentType") != get_component_type<T>())
        throw std::runtime_error("Unexpected accessor componentType.");

    // Handle sparse.
    if (accessor_json.contains("sparse")) {
        switch (accessor_json["sparse"].at("indices").at("componentType").get<GLenum>()) {
        case GL_UNSIGNED_BYTE:
            return read_from_sparse_buffer_view<uint8_t, T>(
                args, accessor_json["sparse"]
            );
            break;
        case GL_UNSIGNED_SHORT:
            return read_from_sparse_buffer_view<uint16_t, T>(
                args, accessor_json["sparse"]
            );
            break;
        case GL_UNSIGNED_INT:
            return read_from_sparse_buffer_view<uint32_t, T>(
                args, accessor_json["sparse"]
            );
            break;
        default:
            throw std::runtime_error("Unknown sparse indices component type.");
            break;
        }
    }
    // Use bufferView if there are no sparse.
    else if (accessor_json.contains("bufferView")) {
        return read_from_fine_buffer_view<T>(
            args, accessor_json["bufferView"].get<uint32_t>(), 0
        );
    }
    // If there are no data source at all, use zeros.
    else {
        return std::vector<T>(accessor_json.at("count").get<uint64_t>(), T());
    }
}

template<typename T>
std::vector<T> load_primitive_attribute(const CommonBufferArgs& args, const json& attributes_json,
        const std::string_view attribute_name) {
    if (!attributes_json.contains(attribute_name))
        return {};
    
    const uint32_t accessor_index = attributes_json[attribute_name].get<uint32_t>();
    const json& accessor_json = args.gltf_json.at("accessors").at(accessor_index);

    return read_from_accessor<T>(args, accessor_json);
}

void construct_mesh_params(GLTF& gltf, const json& gltf_json, std::string_view gltf_path,
                           const std::streamsize bin_chunk_offset) {
    gltf.meshes.clear();

    if (!gltf_json.contains("meshes"))
        return;

    // Create the stream pool
    // (used to not open ifstreams in every read_from_*_buffer_view function). 
    // We could use std::unordered_map instead, but hash computation of
    // entire std::string is slow and we don't have much streams usually.
    std::map<std::string_view, std::ifstream> stream_pool;
    const CommonBufferArgs args {
        gltf_json, gltf_path, bin_chunk_offset, stream_pool
    };
    
    gltf.meshes.reserve(gltf_json["meshes"].size());
    for (const json& mesh_json : gltf_json["meshes"]) {
        GLTF::MeshParameters result;
        
        // Get the primitive.
        // TODO: Support multiple primitives.
        if (mesh_json.at("primitives").size() != 1) {
            throw std::runtime_error("Unsupported amount of primitives in one mesh. "
                    "Only 1 per mesh supported.");
        }
        
        const json& prim_json = mesh_json.at("primitives").at(0);

        // Check some primitive values.
        // TODO: Support other primitive modes.
        if (get_optional<GLenum>(prim_json, "mode", 4) != 4) {
            throw std::runtime_error("Unsupported primitive mode. "
                    "Only TRIANGLES (4) is supported.");
        }
        // TODO: Support primitives without a material.
        if (!prim_json.contains("material")) {
            throw std::runtime_error("Primitives without a material are unsupported.");
        }

        result.material_index = prim_json["material"].get<uint32_t>();

        // Check attributes.
        if (!prim_json.contains("attributes"))
            throw std::runtime_error("The primitive does not contain attributes.");
        if (!prim_json["attributes"].contains("POSITION"))
            throw std::runtime_error("The POSITION attribute is not specified.");
        
        const json& attributes_json = prim_json["attributes"];
        
        // Load positions.
        result.vertices = load_primitive_attribute<glm::vec3>(
            args, attributes_json, "POSITION"
        );
        // Load UV (texture) coordinates.
        result.uvs = load_primitive_attribute<glm::vec2>(
            args, attributes_json, "TEXCOORD_0"
        );
        // Load normals.
        result.normals = load_primitive_attribute<glm::vec3>(
            args, attributes_json, "NORMAL"
        );
        // Load tangents.
        result.tangents = load_primitive_attribute<glm::vec4>(
            args, attributes_json, "TANGENT"
        );
        // Load indices.
        if (prim_json.contains("indices")) {
            const json& accessor_json = gltf_json.at("accessors").at(prim_json["indices"].get<uint32_t>());
            const GLenum component_type = accessor_json.at("componentType").get<GLenum>();

            switch (component_type) {
            case GL_UNSIGNED_SHORT:
                result.indices = read_from_accessor<uint16_t>(
                    args, accessor_json
                );
                break;
            case GL_UNSIGNED_INT:
                result.indices = read_from_accessor<uint32_t>(
                    args, accessor_json
                );
            default:
                throw std::runtime_error("Invalid accessor component type for indices.");
            }
        }

        gltf.meshes.push_back(result);
    }    
}

GLTF::Node construct_node_and_children_params(const json& gltf_json,
        const uint32_t node_index) {
    const json& node_json = gltf_json.at("nodes").at(node_index);

    GLTF::Node result;

    // Process the name.
    result.name = get_optional<std::string>(node_json, "name", "Unnamed");

    // Process the spatial parameters.
    const bool uses_matrix_only {
        !node_json.contains("translation") &&
        !node_json.contains("rotation") &&
        !node_json.contains("scale") &&
        node_json.contains("matrix")
    };
    if (uses_matrix_only) {
        // TODO: Support it.
        throw std::runtime_error("Node matrix transform is not supported.");
    }
    result.spatial_params = {
        get_optional<glm::vec3>(node_json, "translation", DEFAULT_TRANSLATION),
        get_optional<glm::vec3>(node_json, "scale", DEFAULT_SCALE),
        get_optional<glm::quat>(node_json, "rotation", DEFAULT_ROTATION)
    };

    // Process the mesh index.
    result.mesh_index = get_optional<uint32_t>(node_json, "mesh");

    // Process children.
    if (node_json.contains("children")) {
        result.children.reserve(node_json["children"].size());
        for (const uint32_t cur_child_index : node_json["children"]) {
            result.children.push_back(
                construct_node_and_children_params(gltf_json, cur_child_index)
            );
        }
    }

    return result;
}

void construct_node_params(GLTF& gltf, const json& gltf_json) {
    if (!gltf_json.contains("nodes"))
        return;
    
    // Use the construct_node_and_children_params functions on all
    // root nodes (the function above will recursively process all
    // children too). However, not all nodes are roots. We don't
    // know parent of each JSON node, but we know children of each
    // node. So, create a set of all nodes that were found as children
    // and then iterate through all nodes that are not occurred in
    // the non_root_nodes set.
    std::unordered_set<uint32_t> non_root_nodes;
    for (const json& cur_node_json : gltf_json["nodes"]) {
        if (cur_node_json.contains("children") &&
                cur_node_json["children"].size() > 0) {
            non_root_nodes.insert(
                cur_node_json["children"].begin(),
                cur_node_json["children"].end()
            );
        }
    }

    for (uint32_t i = 0; i < gltf_json["nodes"].size(); i++) {
        if (non_root_nodes.contains(i))
            continue;
        
        gltf.nodes.push_back(construct_node_and_children_params(
            gltf_json, i
        ));
    }
}

struct ChunkMetadata {
    uint32_t length;
    uint32_t type;
};

void align_stream(std::istream& stream, const std::streamsize boundary) {
    if (boundary <= 1)
        return;

    stream.seekg(
        (stream.tellg() + boundary - static_cast<std::streamsize>(1)) / boundary * boundary
    );
}

GLTF::GLTF(std::string_view file_path) {
    std::ifstream stream;
    stream.exceptions(std::ifstream::failbit);
    stream.open(std::string(file_path), std::ios::in | std::ios::binary);

    // Read the header and check the magic number.
    Header header;
    stream.read(reinterpret_cast<char*>(&header), sizeof(Header));
    if (header.magic != GLTF_MAGIC)
        throw std::runtime_error("The magic bytes are invalid.");
    if (header.version != GLB_VERSION)
        throw std::runtime_error("Unsupported version of the glTF.");

    // Read and parse the JSON chunk.
    align_stream(stream, 4);
    ChunkMetadata json_chunk_meta;
    stream.read(reinterpret_cast<char*>(&json_chunk_meta), sizeof(ChunkMetadata));
    if (json_chunk_meta.type != CHUNK_TYPE_JSON)
        throw std::runtime_error("The first chunk is not in JSON type.");
    std::string json_string;
    json_string.resize(json_chunk_meta.length);
    stream.read(&json_string[0], json_chunk_meta.length);
    const json json_chunk = json::parse(json_string);

    // Check for unsupported extensions.
    if (json_chunk.contains("extensionsRequired")) {
        for (const json& cur_extension : json_chunk["extensionsRequired"]) {
            const bool is_extension_supported {
                std::find(SUPPORTED_EXTENSIONS.begin(), SUPPORTED_EXTENSIONS.end(), cur_extension) !=
                SUPPORTED_EXTENSIONS.end()
            };
            
            if (!is_extension_supported)
                throw std::runtime_error("Unsupported extension(s) are required by this glTF file.");
        }
    }

    // Parse the binary chunk metadata.
    align_stream(stream, 4);
    ChunkMetadata bin_chunk_meta;
    stream.read(reinterpret_cast<char*>(&bin_chunk_meta), sizeof(ChunkMetadata));
    if (bin_chunk_meta.type != CHUNK_TYPE_BIN)
        throw std::runtime_error("The second chunk is not in BIN type.");

    // Use JSONChunk to construct the final glTF.
    construct_texture_params(*this, json_chunk, file_path, stream.tellg());
    construct_material_params(*this, json_chunk);
    construct_mesh_params(*this, json_chunk, file_path, stream.tellg());
    construct_node_params(*this, json_chunk);
}
