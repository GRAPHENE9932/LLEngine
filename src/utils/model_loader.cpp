#include <fstream>
#include <sstream>
#include <array>

#include "utils.hpp"

void utils::load_wavefront_obj(std::string file_path, std::vector<glm::vec3>& vertices_out,
                               std::vector<glm::vec2>& uvs_out, std::vector<glm::vec3>& normals_out,
                               bool invert_tex_v) {
    std::ifstream stream(file_path, std::ios::in);
    if (!stream)
        throw std::runtime_error("Failed to read the .obj file. File path: " + file_path);

    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;

    std::string line;
    while (std::getline(stream, line)) {
        std::istringstream ss(line);

        // Process the first token.
        std::string first_token;
        ss >> first_token;

        // It's a comment. Skip.
        if (first_token == "#") {
            continue;
        }
        // I don't support materials, lines, objects, groups and smooth shading. Skip.
        else if (
            first_token == "mtllib" || first_token == "usemtl" || first_token == "l" ||
            first_token == "o" || first_token == "g" || first_token == "s"
        ) {
            continue;
        }
        // The vertex.
        else if (first_token == "v") {
            glm::vec3 cur_vertex;
            ss >> cur_vertex.x;
            ss >> cur_vertex.y;
            ss >> cur_vertex.z;
            if (!ss)
                throw std::runtime_error(
                    "Failed to parse the .obj file:\n"
                    "error on vertex coordinates parsing.\n"
                    "File path: " + file_path
                );
            vertices.push_back(cur_vertex);
        }
        // Texture coordinates (UV).
        else if (first_token == "vt") {
            glm::vec2 cur_uv;
            ss >> cur_uv.x;
            ss >> cur_uv.y;
            if (!ss)
                throw std::runtime_error(
                    "Failed to parse the .obj file:\n"
                    "error on texture coordinates parsing.\n"
                    "File path: " + file_path
                );
            if (invert_tex_v)
                cur_uv.y = 1.0f - cur_uv.y;
            uvs.push_back(cur_uv);
        }
        // Vertex normals.
        else if (first_token == "vn") {
            glm::vec3 cur_normal;
            ss >> cur_normal.x;
            ss >> cur_normal.y;
            ss >> cur_normal.z;
            if (!ss)
                throw std::runtime_error(
                    "Failed to parse the .obj file:\n"
                    "error on vertex normals parsing.\n"
                    "File path: " + file_path
                );
            normals.push_back(cur_normal);
        }
        // Faces.
        else if (first_token == "f") {
            for (int i = 0; i < 3; i++) {
                std::string indices_string;
                ss >> indices_string;
                if (!ss)
                    throw std::runtime_error(
                        "Failed to parse the .obj file:\n"
                        "error on faces parsing.\n"
                        "File path: " + file_path
                    );

                std::array<int, 3> indices;
                std::istringstream indices_ss(indices_string);
                for (int i = 0; i < 3; i++) {
                    std::string index_string;
                    if (!std::getline(indices_ss, index_string, '/'))
                        throw std::runtime_error(
                            "Failed to parse the .obj file:\n"
                            "error on face indices parsing (not enough of them).\n"
                            "File path: " + file_path
                        );
                    indices[i] = std::stoi(index_string);
                }

                vertices_out.push_back(vertices.at(indices[0] - 1));
                uvs_out.push_back(uvs.at(indices[1] - 1));
                normals_out.push_back(normals.at(indices[2] - 1));
            }
        }
    }
}
