#include "utils/glm_bullet_conversion.hpp"
#include "physics/shapes/Shape.hpp"
#include "physics/BulletPhysicsServer.hpp"
#include "nodes/physics/BulletRigidBodyNode.hpp"

#include <bullet/btBulletDynamicsCommon.h>

#include <cmath>

Transform get_relative_transform(const BulletRigidBodyNode& node,
                                 const Transform& parent_transform) {
    const auto global_transform {node.get_global_transform()};

    return {
        global_transform.translation - parent_transform.translation,
        node.get_scale(),
        glm::inverse(parent_transform.rotation) * global_transform.rotation
    };
}

BulletRigidBodyNode::BulletRigidBodyNode(
    BulletPhysicsServer& bps,
    const std::shared_ptr<Shape>& shape,
    float mass, const Transform& transform
) : shape(shape), bps(bps) {
    if (mass < 0.0f || !std::isfinite(mass)) {
        throw std::invalid_argument("Specified mass is invalid.");
    }

    btTransform bt_transform(
        glm_quat_to_bullet(transform.rotation),
        glm_vec3_to_bullet(transform.translation)
    );
    BulletRigidBodyNode::set_scale(transform.scale);

    std::unique_ptr<btDefaultMotionState> motion_state =
        std::make_unique<btDefaultMotionState>(bt_transform);
    btVector3 local_inertia(btScalar(0.0f), btScalar(0.0f), btScalar(0.0f));
    if (mass != 0.0f) {
        shape->get_bullet_collision_shape()->calculateLocalInertia(btScalar(mass), local_inertia);
    }

    btRigidBody::btRigidBodyConstructionInfo rb_info(
        btScalar(mass),
        motion_state.release(),
        shape->get_bullet_collision_shape(),
        local_inertia
    );
    bt_rigid_body = std::make_unique<btRigidBody>(rb_info);

    bps.register_rigid_body(this);
}

BulletRigidBodyNode::~BulletRigidBodyNode() noexcept {
    bps.unregister_rigid_body(this);

    if (get_bt_rigid_body()->getMotionState()) {
        delete get_bt_rigid_body()->getMotionState();
    }
}

void BulletRigidBodyNode::before_simulation_step() {
    if (get_parent()) {
        const auto current_parent_transform {get_parent()->get_global_transform()};
        
        if (current_parent_transform != previous_parent_transform) {
            // Now we know that parent's global transform has changed.
            // We don't store the local transform here, but only the global one.
            // So, calculate the local transform relatively to the previous
            // parent's transform and apply it relatively to the new
            // parent's transform.
            const auto local_transform {
                get_relative_transform(*this, previous_parent_transform)
            };
            set_global_transform(local_transform * current_parent_transform);
        }
    }
}

void BulletRigidBodyNode::set_translation(const glm::vec3& new_trans) {
    const auto parent {get_parent()};

    glm::vec3 new_global_position;
    if (parent) {
        new_global_position = parent->get_global_position() + new_trans;
    }
    else {
        new_global_position = new_trans;
    }

    btTransform cur_transform;
    get_bt_rigid_body()->getMotionState()->getWorldTransform(cur_transform);
    cur_transform.setOrigin(glm_vec3_to_bullet(new_global_position));
    get_bt_rigid_body()->getMotionState()->setWorldTransform(cur_transform);
}

void BulletRigidBodyNode::translate(const glm::vec3& translation) {
    btTransform cur_transform;
    get_bt_rigid_body()->getMotionState()->getWorldTransform(cur_transform);
    cur_transform.setOrigin(cur_transform.getOrigin() + glm_vec3_to_bullet(translation));
    get_bt_rigid_body()->getMotionState()->setWorldTransform(cur_transform);
}

void BulletRigidBodyNode::set_scale(const glm::vec3& new_scale) {
    if (new_scale == get_scale()) {
        return;
    }

    const glm::vec3 relative_scale = new_scale / get_scale();

    // Make the new shape and scale it.
    std::shared_ptr<Shape> shape_copy = shape->deep_copy();
    shape_copy->scale(relative_scale);
    this->set_shape(shape_copy);

    scale = new_scale;
}

void BulletRigidBodyNode::set_rotation(const glm::quat& new_rotation) {
    const auto parent {get_parent()};

    glm::quat new_global_rotation;
    if (parent) {
        new_global_rotation = new_rotation * parent->get_global_rotation();
    }
    else {
        new_global_rotation = new_rotation;
    }

    btTransform cur_transform;
    get_bt_rigid_body()->getMotionState()->getWorldTransform(cur_transform);
    cur_transform.setRotation(glm_quat_to_bullet(new_global_rotation));
    get_bt_rigid_body()->getMotionState()->setWorldTransform(cur_transform);
}

void BulletRigidBodyNode::set_transform(const Transform &new_transform) {
    const auto parent {get_parent()};

    btTransform new_bt_transform;
    if (parent) {
        new_bt_transform.setOrigin(glm_vec3_to_bullet(
            parent->get_global_position() + new_transform.translation
        ));
        new_bt_transform.setRotation(glm_quat_to_bullet(
            new_transform.rotation * parent->get_global_rotation()
        ));
    }
    else {
        new_bt_transform.setOrigin(glm_vec3_to_bullet(
            new_transform.translation
        ));
        new_bt_transform.setRotation(glm_quat_to_bullet(
            new_transform.rotation
        ));
    }

    get_bt_rigid_body()->getMotionState()->setWorldTransform(new_bt_transform);

    set_scale(new_transform.scale);
}

void BulletRigidBodyNode::set_global_scale(const glm::vec3& new_scale) {
    const auto parent {get_parent()};

    if (parent) {
        set_scale(new_scale / parent->get_global_scale());
    }
    else {
        set_scale(new_scale);
    }
}

void BulletRigidBodyNode::set_global_transform(const Transform& new_transform) {
    btTransform new_bt_transform;
    new_bt_transform.setOrigin(glm_vec3_to_bullet(new_transform.translation));
    new_bt_transform.setRotation(glm_quat_to_bullet(new_transform.rotation));
    get_bt_rigid_body()->getMotionState()->setWorldTransform(new_bt_transform);

    set_global_scale(new_transform.scale);
}

glm::vec3 BulletRigidBodyNode::get_translation() const noexcept {
    const auto parent {get_parent()};

    if (parent) {
        return get_global_position() - parent->get_global_position();
    }
    else {
        return get_global_position();
    }
}

glm::vec3 BulletRigidBodyNode::get_global_position() const noexcept {
    btTransform cur_transform;
    get_bt_rigid_body()->getMotionState()->getWorldTransform(cur_transform);
    return bullet_vec3_to_glm<float>(cur_transform.getOrigin());
}

glm::vec3 BulletRigidBodyNode::get_scale() const noexcept {
    return scale;
}

glm::vec3 BulletRigidBodyNode::get_global_scale() const noexcept {
    const auto parent {get_parent()};

    if (parent) {
        return parent->get_global_scale() * get_scale();
    }
    else {
        return get_scale();
    }
}

glm::quat BulletRigidBodyNode::get_rotation() const noexcept {
    const auto parent {get_parent()};

    if (parent) {
        return glm::inverse(parent->get_global_rotation()) * get_global_rotation();
    }
    else {
        return get_global_rotation();
    }
}

glm::quat BulletRigidBodyNode::get_global_rotation() const noexcept {
    btTransform cur_transform;
    get_bt_rigid_body()->getMotionState()->getWorldTransform(cur_transform);
    return bullet_quat_to_glm<float>(cur_transform.getRotation());
}

Transform BulletRigidBodyNode::get_transform() const noexcept {
    const auto parent {get_parent()};

    if (parent) {
        return get_relative_transform(*this, get_parent()->get_global_transform());
    }
    else {
        return get_global_transform();
    }
}

Transform BulletRigidBodyNode::get_global_transform() const noexcept {
    btTransform cur_transform;
    get_bt_rigid_body()->getMotionState()->getWorldTransform(cur_transform);

    return {
        bullet_vec3_to_glm<float>(cur_transform.getOrigin()),
        get_global_scale(),
        bullet_quat_to_glm<float>(cur_transform.getRotation())
    };
}

glm::mat4 BulletRigidBodyNode::get_local_matrix() const noexcept {
    return get_transform().calculate_matrix();
}

glm::mat4 BulletRigidBodyNode::get_global_matrix() const noexcept {
    return get_global_transform().calculate_matrix();
}

void BulletRigidBodyNode::set_shape(const std::shared_ptr<Shape>& shape) {
    this->shape = shape;
}

void BulletRigidBodyNode::set_mass(float new_mass) {
    if (new_mass < 0.0f || !std::isfinite(new_mass)) {
        throw std::invalid_argument("Specified mass is invalid.");
    }

    btVector3 local_inertia(btScalar(0.0f), btScalar(0.0f), btScalar(0.0f));
    if (new_mass != 0.0f) {
        shape->get_bullet_collision_shape()->calculateLocalInertia(btScalar(new_mass), local_inertia);
    }

    get_bt_rigid_body()->setMassProps(btScalar(new_mass), local_inertia);
}

float BulletRigidBodyNode::get_mass() const noexcept {
    return bt_rigid_body->getMass();
}
