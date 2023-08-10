#pragma once

#include "datatypes.hpp"

namespace llengine {
class ManagedFramebufferID {
public:
    ManagedFramebufferID();
    ManagedFramebufferID(FramebufferID id);
    ManagedFramebufferID(const ManagedFramebufferID& other) = delete;
    ManagedFramebufferID(ManagedFramebufferID&& other) noexcept;
    ~ManagedFramebufferID();

    operator FramebufferID() const {
        return get();
    }

    void set_id(FramebufferID id);
    [[nodiscard]] FramebufferID get() const {
        return id;
    }
    [[nodiscard]] FramebufferID& get_ref() {
        return id;
    }

    void delete_framebuffer();

private:
    FramebufferID id = 0;
};
}