#pragma once

#include <vector>
#include <memory>
#include <variant>

#include <glm/vec2.hpp> // glm::vec2
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4

#include "datatypes.hpp"

namespace llengine {
class Mesh {
public:
    Mesh() = default;
    Mesh(const Mesh& other);
    Mesh(Mesh&& other) noexcept;
    ~Mesh();

    Mesh& operator=(const Mesh& other);
    Mesh& operator=(Mesh&& other) noexcept;

    [[nodiscard]] BufferID get_indices_id() const { return indices_id; }
    [[nodiscard]] BufferID get_vertices_id() const { return vertices_id; }
    [[nodiscard]] BufferID get_uvs_id() const { return uvs_id; }
    [[nodiscard]] BufferID get_normals_id() const { return normals_id; }
    [[nodiscard]] BufferID get_tangents_id() const { return tangents_id; }

    [[nodiscard]] size_t get_amount_of_vertices() const;
    [[nodiscard]] GraphicsAPIEnum get_indices_type() const;

    [[nodiscard]] inline bool is_initialized() const noexcept {
        return vertices_id != 0;
    }
    /**
     * @brief Binds this Mesh's VAO. If this Mesh does not have a VAO,
     * one will be created.
     *
     * Vertex attribute array index 0 corresponds to vertex positions,
     * 1 to UVs,
     * 2 to normals,
     * 3 to tangents.
     */
    void bind_vao(bool enable_uv = true, bool enable_normals = true, bool enable_tangents = true) const;
    void unbind_vao(bool unbind_uv, bool unbind_normals, bool unbind_tangents) const;

    template<typename T>
    void set_indices(const std::vector<T>& new_indices);
    void set_vertices(const std::vector<glm::vec3>& new_vertices);
    void set_uvs(const std::vector<glm::vec2>& new_uvs);
    void set_normals(const std::vector<glm::vec3>& new_normals);
    void set_tangents(const std::vector<glm::vec4>& new_tangents);

    void index_data();

    [[nodiscard]] bool is_indexed() const {
        return get_indices_id() != 0;
    }

    /**
     * @brief Get the indexed 2x2x2 cube mesh with UVs, normals and tangents.
     *
     * The Mesh object is being created only on the first call, after that
     * returns shared_ptr with the same object.
     */
    [[nodiscard]] static std::shared_ptr<const Mesh> get_cube();

    /**
     * @brief Get the indexed 2x2x2 cube mesh that can be used as skybox mesh.
     *
     * This Mesh DOES NOT contain UVs, normals or tangents. Triangles are
     * faced inwards, so with enabled backface culling the cube will be
     * visible only from inside.
     */
    [[nodiscard]] static std::shared_ptr<const Mesh> get_skybox_cube();

    /**
     * @brief Get the not indexed 2x2 quad mesh with UVs.
     *
     * This Mesh DOES NOT contain normals and tangents. Triangles are front-facing.
     * Vertex positions have constant Z=0 (the quad is in XY plane).
     */
    [[nodiscard]] static std::shared_ptr<const Mesh> get_quad();

private:
    class HandledBufferID {
    public:
        HandledBufferID();
        HandledBufferID(BufferID buffer_id);
        HandledBufferID(const HandledBufferID& other) = delete;
        HandledBufferID(HandledBufferID&& other);
        ~HandledBufferID();

        operator BufferID() const {
            return get();
        }

        [[nodiscard]] BufferID get() const {
            return buffer_id;
        }

        HandledBufferID& operator=(const HandledBufferID& other) = delete;
        HandledBufferID& operator=(HandledBufferID&& other);
        HandledBufferID& operator=(BufferID buffer_id);

    private:
        BufferID buffer_id;
    };

    HandledBufferID indices_id = 0, vertices_id = 0, uvs_id = 0,
           normals_id = 0, tangents_id = 0;
    mutable VertexArrayID vao_id = 0;

    std::variant<std::vector<uint16_t>, std::vector<uint32_t>> indices;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec4> tangents;

    template<typename T> void index_data();
    void reset_vao_if_needed() const;
    void initialize_vao() const;
};
}