#pragma once

#include "datatypes.hpp"

namespace llengine {
class ManagedRenderbufferID {
public:
    ManagedRenderbufferID();
    ManagedRenderbufferID(FramebufferID id);
    ManagedRenderbufferID(const ManagedRenderbufferID& other) = delete;
    ManagedRenderbufferID(ManagedRenderbufferID&& other) noexcept;
    ~ManagedRenderbufferID();

    operator RenderbufferID() const {
        return get();
    }

    void set_id(RenderbufferID id);
    [[nodiscard]] RenderbufferID get() const {
        return id;
    }
    [[nodiscard]] RenderbufferID& get_ref() {
        return id;
    }

    void delete_renderbuffer();

private:
    RenderbufferID id = 0;
};
}