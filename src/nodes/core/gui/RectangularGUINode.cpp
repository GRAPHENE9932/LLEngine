#include "RectangularGUINode.hpp"
#include "RenderingServer.hpp"
#include "utils/primitive_meshes.hpp"

[[nodiscard]] glm::vec2 RectangularGUINode::get_origin() const {
    const glm::vec2 size = get_size();
    glm::vec2 result;

    switch (origin_x) {
    case OriginX::LEFT:
        result.x = 0.0f;
        break;
    case OriginX::CENTER:
        result.x = size.x / 2.0f;
        break;
    case OriginX::RIGHT:
        result.x = size.x;
        break;
    }

    switch (origin_y) {
    case OriginY::BOTTOM:
        result.y = size.y;
        break;
    case OriginY::CENTER:
        result.y = size.y / 2.0f;
        break;
    case OriginY::TOP:
        result.y = 0.0;
        break;
    }

    return result;
}

[[nodiscard]] bool RectangularGUINode::contains_point(glm::vec2 point) const {
    glm::vec2 top_left = static_cast<glm::vec2>(get_absolute_transform().to_screen_coordinates(
        rs.get_window().get_window_size()
    )) - get_origin();

    glm::vec2 bottom_right = top_left + get_size();

    return point.x >= top_left.x && point.x <= bottom_right.x &&
        point.y >= top_left.y && point.y <= bottom_right.y;
}

void RectangularGUINode::draw_texture_part(
    const Texture& texture, glm::vec2 pos_offset_in_px, glm::vec2 tex_offset_in_px,
    glm::vec2 quad_size_in_px, glm::vec2 tex_part_size_in_px
) {
    const glm::vec2 viewport_size = rs.get_window().get_window_size();

    auto absolute_transform = get_absolute_transform();
    // The quad mesh is in range from -1.0 to 1.0, but
    // not from 0.0 to 1.0. So, to pivot it to the corner, add half of a size.
    absolute_transform.position_offset += pos_offset_in_px + quad_size_in_px / 2.0f;
    const glm::mat4 mvp = glm::translate(
        absolute_transform.to_opengl_position(rs.get_window().get_window_size())
    ) * glm::scale(glm::vec3(quad_size_in_px / viewport_size, 1.0f));

    glm::vec2 uv_scale = tex_part_size_in_px / static_cast<glm::vec2>(texture.get_size());
    glm::vec2 uv_offset = tex_offset_in_px / static_cast<glm::vec2>(texture.get_size());
    uv_offset.y += uv_scale.y; // Invert UV's y. If (scale * y) goes from 0 to scale when y goes from 0 to 1,
    uv_scale.y = -uv_scale.y; // then (scale - scale * y) goes from scale to 0.

    rs.get_shader_holder().get_gui_rectangle_shader().use_shader(
        texture, mvp, uv_scale, uv_offset, {1.0f, 1.0f, 1.0f, 1.0f}
    );

    auto mesh = primitives::get_quad();
    mesh->bind_vao(true, false, false);
    
    if (mesh->is_indexed()) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->get_indices_id());
        glDrawElements(GL_TRIANGLES, mesh->get_amount_of_vertices(), mesh->get_indices_type(), 0);
    }
    else {
        glBindBuffer(GL_ARRAY_BUFFER, mesh->get_vertices_id());
        glDrawArrays(GL_TRIANGLES, 0, mesh->get_amount_of_vertices());
    }
    rs.report_about_drawn_triangles(mesh->get_amount_of_vertices() / 3);

    mesh->unbind_vao();
}

void RectangularGUINode::draw_rectangle(const GUITexture& texture) {
    const std::array<glm::vec2, 3> tex_part_sizes {
        glm::vec2(texture.left_border, texture.top_border),
        glm::vec2(texture->get_size().x - texture.left_border - texture.right_border, texture->get_size().y - texture.top_border - texture.bottom_border),
        glm::vec2(texture.right_border, texture.bottom_border)
    };
    const std::array<glm::vec2, 3> quad_sizes {
        glm::vec2(texture.left_border, texture.top_border),
        glm::vec2(get_size() - texture.left_border - texture.right_border),
        glm::vec2(texture.right_border, texture.bottom_border)
    };
    glm::vec2 offset_in_tex {0.0f, 0.0f};
    glm::vec2 offset_of_quad {0.0f, 0.0f};
    for (std::size_t x = 0; x < 3; x++) {
        for (std::size_t y = 0; y < 3; y++) {
            draw_texture_part(
                *texture,
                offset_of_quad - get_origin(),
                offset_in_tex,
                {quad_sizes[x].x, quad_sizes[y].y},
                {tex_part_sizes[x].x, tex_part_sizes[y].y}
            );
            offset_in_tex.y += tex_part_sizes[y].y;
            offset_of_quad.y += quad_sizes[y].y;
        }
        offset_in_tex.x += tex_part_sizes[x].x;
        offset_in_tex.y = 0.0f;
        offset_of_quad.x += quad_sizes[x].x;
        offset_of_quad.y = 0.0f;
    }
}
