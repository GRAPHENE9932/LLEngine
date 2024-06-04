#include "nodes/SpatialNode.hpp"
#include "utils/glm_bullet_conversion.hpp"
#include "physics/shapes/Shape.hpp"
#include "physics/BulletPhysicsServer.hpp"
#include "nodes/physics/BulletRigidBodyNode.hpp"

#include <bullet/btBulletDynamicsCommon.h>

#include <cmath>

using namespace llengine;

static Transform get_relative_transform(const BulletRigidBodyNode& node,
                                 const Transform& parent_transform) {
    const auto global_transform {node.get_global_transform()};

    return {
        global_transform.translation - parent_transform.translation,
        node.get_scale(),
        glm::inverse(parent_transform.rotation) * global_transform.rotation
    };
}

BulletRigidBodyNode::BulletRigidBodyNode() {
    bt_rigid_body = nullptr;
}

BulletRigidBodyNode::~BulletRigidBodyNode() noexcept {
    if (auto bullet_physics_server = get_bullet_physics_server_optional()) {
        bullet_physics_server->unregister_rigid_body(this);
    }

    if (get_bt_rigid_body() != nullptr && get_bt_rigid_body()->getMotionState()) {
        delete get_bt_rigid_body()->getMotionState();
    }
}

void BulletRigidBodyNode::before_simulation_step() {
    if (get_parent()) {
        const auto current_parent_transform {get_parent()->get_global_transform()};

        if (current_parent_transform != previous_parent_transform) {
            // Bullet rigid bodies do not obey the node system with relative transforms we have.
            // The consequence is unchanged global position when parent's transform had changed.
            // To fix this, every simulation step we apply difference between previous and current parent transforms
            // to our global transform.
            set_global_transform((current_parent_transform / previous_parent_transform) * get_global_transform());

            previous_parent_transform = current_parent_transform;
        }
    }

    for (const auto& args : queued_impulse_application_calls) {
        apply_impulse(args.first, args.second);
    }
    queued_impulse_application_calls.clear();

    if (!is_static()) {
        invalidate_transform_cache();
    }
}

void BulletRigidBodyNode::set_translation(const glm::vec3& new_trans) {
    if (!get_bt_rigid_body()) {
        cached_set_translation = new_trans;
        return;
    }

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
    get_bt_rigid_body()->setWorldTransform(cur_transform);
    get_bt_rigid_body()->getMotionState()->setWorldTransform(cur_transform);

    invalidate_transform_cache();
}

void BulletRigidBodyNode::translate(const glm::vec3& translation) {
    if (!get_bt_rigid_body()) {
        cached_set_translation += translation;
        return;
    }

    btTransform cur_transform;
    get_bt_rigid_body()->getMotionState()->getWorldTransform(cur_transform);
    cur_transform.setOrigin(cur_transform.getOrigin() + glm_vec3_to_bullet(translation));
    get_bt_rigid_body()->setWorldTransform(cur_transform);
    get_bt_rigid_body()->getMotionState()->setWorldTransform(cur_transform);

    invalidate_transform_cache();
}

void BulletRigidBodyNode::set_scale(const glm::vec3& new_scale) {
    if (new_scale == get_scale()) {
        return;
    }

    if (!get_bt_rigid_body()) {
        scale = new_scale;
        return;
    }

    const glm::vec3 relative_scale = new_scale / get_scale();

    // Make the new shape and scale it.
    std::shared_ptr<Shape> shape_copy = shape->copy();
    shape_copy->scale(relative_scale);
    this->set_shape(shape_copy);

    scale = new_scale;

    invalidate_transform_cache();
}

void BulletRigidBodyNode::set_rotation(const glm::quat& new_rotation) {
    if (!get_bt_rigid_body()) {
        cached_set_rotation = new_rotation;
        return;
    }

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
    get_bt_rigid_body()->setWorldTransform(cur_transform);
    get_bt_rigid_body()->getMotionState()->setWorldTransform(cur_transform);

    invalidate_transform_cache();
}

void BulletRigidBodyNode::set_transform(const Transform &new_transform) {
    if (!get_bt_rigid_body()) {
        cached_set_translation = new_transform.translation;
        cached_set_rotation = new_transform.rotation;
        scale = new_transform.scale;
        return;
    }

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

    get_bt_rigid_body()->setWorldTransform(new_bt_transform);
    get_bt_rigid_body()->getMotionState()->setWorldTransform(new_bt_transform);

    set_scale(new_transform.scale);

    invalidate_transform_cache();
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
    if (!get_bt_rigid_body()) {
        Transform parent_transform = Transform();
        if (get_parent()) {
            parent_transform = get_parent()->get_global_transform();
        }

        Transform transform_to_set = new_transform / parent_transform;
        cached_set_translation = transform_to_set.translation;
        cached_set_rotation = transform_to_set.rotation;
        scale = transform_to_set.scale;
        return;
    }

    btTransform new_bt_transform;
    new_bt_transform.setOrigin(glm_vec3_to_bullet(new_transform.translation));
    new_bt_transform.setRotation(glm_quat_to_bullet(new_transform.rotation));
    get_bt_rigid_body()->setWorldTransform(new_bt_transform);
    get_bt_rigid_body()->getMotionState()->setWorldTransform(new_bt_transform);

    set_global_scale(new_transform.scale);
}

glm::vec3 BulletRigidBodyNode::get_translation() const noexcept {
    if (!get_bt_rigid_body()) {
        return cached_set_translation;
    }

    const auto parent {get_parent()};

    if (parent) {
        return get_global_position() - parent->get_global_position();
    }
    else {
        return get_global_position();
    }
}

glm::vec3 BulletRigidBodyNode::get_global_position() const noexcept {
    if (!get_bt_rigid_body()) {
        const auto parent { get_parent() };

        if (parent) {
            return cached_set_translation + parent->get_global_position();
        }
        else {
            return cached_set_translation;
        }
    }

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
    if (!get_bt_rigid_body()) {
        const auto parent { get_parent() };

        if (parent) {
            return cached_set_rotation * parent->get_global_rotation();
        }
        else {
            return cached_set_rotation;
        }
    }

    btTransform cur_transform;
    get_bt_rigid_body()->getMotionState()->getWorldTransform(cur_transform);
    return bullet_quat_to_glm<float>(cur_transform.getRotation());
}

Transform BulletRigidBodyNode::get_transform() const noexcept {
    if (!get_bt_rigid_body()) {
        return {
            cached_set_translation,
            scale,
            cached_set_rotation
        };
    }

    const auto parent {get_parent()};

    if (parent) {
        return get_relative_transform(*this, get_parent()->get_global_transform());
    }
    else {
        return get_global_transform();
    }
}

Transform BulletRigidBodyNode::get_global_transform() const noexcept {
    if (!get_bt_rigid_body()) {
        const auto parent { get_parent() };

        if (parent) {
            return get_transform() * parent->get_global_transform();
        }
        else {
            return get_transform();
        }
    }

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
    if (!cached_global_matrix.has_value()) {
        cached_global_matrix = get_global_transform().calculate_matrix();
    }

    return *cached_global_matrix;
}

void BulletRigidBodyNode::set_shape(const std::shared_ptr<Shape>& shape) {
    this->shape = shape;
    if (!get_bt_rigid_body()) {
        create_bullet_body(*shape, {cached_set_translation, scale, cached_set_rotation}, cached_set_mass);
        return;
    }

    get_bt_rigid_body()->setCollisionShape(shape->get_bullet_collision_shape());
}

void BulletRigidBodyNode::set_mass(float new_mass) {
    if (new_mass < 0.0f || !std::isfinite(new_mass)) {
        throw std::invalid_argument("Specified mass is invalid.");
    }

    if (!bt_rigid_body) {
        cached_set_mass = new_mass;
        return;
    }

    btVector3 local_inertia(btScalar(0.0f), btScalar(0.0f), btScalar(0.0f));
    if (new_mass != 0.0f) {
        shape->get_bullet_collision_shape()->calculateLocalInertia(btScalar(new_mass), local_inertia);
    }

    get_bt_rigid_body()->setMassProps(btScalar(new_mass), local_inertia);
}

float BulletRigidBodyNode::get_mass() const noexcept {
    if (!get_bt_rigid_body()) {
        return cached_set_mass;
    }
    else {
        return bt_rigid_body->getMass();
    }
}

void BulletRigidBodyNode::queue_apply_impulse(const glm::vec3& impulse_vector, const glm::vec3& point) {
    queued_impulse_application_calls.emplace_back(impulse_vector, point);
}

void BulletRigidBodyNode::apply_impulse(const glm::vec3& impulse_vector, const glm::vec3& point) {
    get_bt_rigid_body()->applyImpulse(
        glm_vec3_to_bullet(impulse_vector),
        glm_vec3_to_bullet(point)
    );
}

void BulletRigidBodyNode::create_bullet_body(const Shape& new_shape, const Transform& transform, float new_mass) {
    btTransform bt_transform(
        glm_quat_to_bullet(transform.rotation),
        glm_vec3_to_bullet(transform.translation)
    );
    BulletRigidBodyNode::set_scale(transform.scale);

    std::unique_ptr<btDefaultMotionState> motion_state =
        std::make_unique<btDefaultMotionState>(bt_transform);
    btVector3 local_inertia(btScalar(0.0f), btScalar(0.0f), btScalar(0.0f));
    if (new_mass != 0.0f) {
        shape->get_bullet_collision_shape()->calculateLocalInertia(btScalar(new_mass), local_inertia);
    }

    btRigidBody::btRigidBodyConstructionInfo rb_info(
        btScalar(new_mass),
        motion_state.release(),
        shape->get_bullet_collision_shape(),
        local_inertia
    );
    bt_rigid_body = std::make_unique<btRigidBody>(rb_info);
}

[[nodiscard]] bool BulletRigidBodyNode::is_contact_checking_enabled() const {
    return check_collisions;
}

void BulletRigidBodyNode::set_collision_checking_enabled(bool enable) {
    check_collisions = enable;
}

void BulletRigidBodyNode::copy_to(Node& node) const {
    SpatialNode::copy_to(node);

    BulletRigidBodyNode& rigid_body = dynamic_cast<BulletRigidBodyNode&>(node);
    rigid_body.set_shape(shape);
    rigid_body.set_mass(get_mass());
    rigid_body.cached_set_mass = cached_set_mass;
    rigid_body.cached_set_rotation = cached_set_rotation;
    rigid_body.cached_set_translation = cached_set_translation;
}

std::unique_ptr<Node> BulletRigidBodyNode::copy() const {
    std::unique_ptr<Node> result { std::make_unique<BulletRigidBodyNode>() };
    copy_to(*result);
    return result;
}

void BulletRigidBodyNode::_on_attachment_to_tree_without_start() {
    SpatialNode::_on_attachment_to_tree_without_start();
    get_bullet_physics_server().register_rigid_body(this);
}

void BulletRigidBodyNode::invalidate_transform_cache() const {
    cached_global_matrix = std::nullopt;
}