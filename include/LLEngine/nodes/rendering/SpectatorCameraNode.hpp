#pragma once

#include "nodes/rendering/CameraNode.hpp" // CameraNode
#include "Key.hpp"

namespace llengine {
class SpectatorCameraNode : public CameraNode {
public:
    static constexpr Key FORWARD_KEY = Key::W;
    static constexpr Key BACKWARD_KEY = Key::S;
    static constexpr Key RIGHT_KEY = Key::D;
    static constexpr Key LEFT_KEY = Key::A;

    float sensivity = 0.001f;
    float speed = 10.0f;

    float x_angle = 0.0f;
    float y_angle = 0.0f;

    void internal_update() override;

    void unlock_mouse();
    void lock_mouse();
    [[nodiscard]] bool is_mouse_locked() const {
        return mouse_locked;
    }

private:
    bool mouse_locked = true;

    void update_rotation();
    void update_position();
};
}