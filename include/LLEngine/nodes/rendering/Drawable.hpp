#pragma once

#include "datatypes.hpp"

namespace llengine {
struct Frustum;
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
    virtual ShaderID get_program_id() const = 0;

    /**
     * @brief Tests if the drawable object is definitely outside the
     * main camera frustum.
     * 
     * May return false even if the object is outside.
     *
     * @return true if and only if the drawable object is 100% outside the main camera frustum.
     * @return false if drawable object is PROBABLY inside the camera frustum.
     */
    [[nodiscard]] virtual bool is_outside_the_frustum(const Frustum& frustum) const {
        return false;
    }
};
}