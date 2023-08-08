#pragma once

#include <GL/glew.h> // GLuint

namespace llengine {
class Drawable {
public:
    virtual void draw() = 0;
    virtual void draw_to_shadow_map() {}
    [[nodiscard]] virtual bool is_enabled() const = 0;
    /**
     * @brief Get the shader program ID from the drawable object if there any.
     * 
     * Must be used only for optimization purposes, to sort by shader program ID.
     *
     * @returns Shader program ID if there are one concrete shader used in this drawable
     * node, or 0 if there are a lot of shaders used.
     */
    virtual GLuint get_program_id() const = 0;
};
}