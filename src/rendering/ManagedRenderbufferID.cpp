#include "rendering/ManagedRenderbufferID.hpp"

#include <GL/glew.h>

namespace llengine {
ManagedRenderbufferID::ManagedRenderbufferID() = default;

ManagedRenderbufferID::ManagedRenderbufferID(RenderbufferID id) : id(id) {}

ManagedRenderbufferID::ManagedRenderbufferID(ManagedRenderbufferID&& other) noexcept {
    set_id(other.id);
    other.id = 0;
}

ManagedRenderbufferID::~ManagedRenderbufferID() {
    delete_renderbuffer();
}

void ManagedRenderbufferID::set_id(RenderbufferID id) {
    this->id = id;
}

void ManagedRenderbufferID::delete_renderbuffer() {
    glDeleteRenderbuffers(1, &id);
    id = 0;
}
}