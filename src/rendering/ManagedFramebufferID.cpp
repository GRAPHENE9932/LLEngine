#include "rendering/ManagedFramebufferID.hpp"

#include <GL/glew.h>

namespace llengine {
ManagedFramebufferID::ManagedFramebufferID() = default;

ManagedFramebufferID::ManagedFramebufferID(FramebufferID id) : id(id) {}

ManagedFramebufferID::ManagedFramebufferID(ManagedFramebufferID&& other) noexcept {
    set_id(other.id);
    other.id = 0;
}

ManagedFramebufferID::~ManagedFramebufferID() {
    delete_framebuffer();
}

ManagedFramebufferID& ManagedFramebufferID::operator=(ManagedFramebufferID&& other) noexcept {
    delete_framebuffer();

    set_id(other.id);
    other.id = 0;

    return *this;
}

void ManagedFramebufferID::set_id(FramebufferID id) {
    this->id = id;
}

void ManagedFramebufferID::delete_framebuffer() {
    glDeleteFramebuffers(1, &id);
    id = 0;
}
}