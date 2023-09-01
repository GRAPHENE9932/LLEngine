#include "physics/BulletPhysicsServer.hpp"
#include "utils/glm_bullet_conversion.hpp"
#include "nodes/physics/BulletRigidBodyNode.hpp"

#include <bullet/BulletCollision/NarrowPhaseCollision/btRaycastCallback.h>
#include <bullet/btBulletDynamicsCommon.h>

#include <algorithm>

using namespace llengine;

BulletPhysicsServer::BulletPhysicsServer() {
    collision_configuration = std::make_unique<btDefaultCollisionConfiguration>();
    collision_dispatcher = std::make_unique<btCollisionDispatcher>(collision_configuration.get());
    broadphase_interface = std::make_unique<btDbvtBroadphase>();
    impulse_constraint_solver = std::make_unique<btSequentialImpulseConstraintSolver>();
    dynamics_world = std::make_unique<btDiscreteDynamicsWorld>(
        collision_dispatcher.get(), broadphase_interface.get(),
        impulse_constraint_solver.get(), collision_configuration.get()
    );
}

BulletPhysicsServer::~BulletPhysicsServer() = default;

void BulletPhysicsServer::do_step(float delta_time) {
    for (BulletRigidBodyNode* cur_rigid_body : rigid_bodies) {
        cur_rigid_body->before_simulation_step();
    }

    bool must_check_for_collisions = false;
    for (const BulletRigidBodyNode* body : rigid_bodies) {
        if (body->is_contact_checking_enabled()) {
            must_check_for_collisions = true;
            break;
        }
    }

    if (must_check_for_collisions) {
        check_for_contacts();
    }

    dynamics_world->stepSimulation(delta_time);
}

void BulletPhysicsServer::check_for_contacts() const {
    int manifolds_count = dynamics_world->getDispatcher()->getNumManifolds();
    for (int i = 0; i < manifolds_count; i++) {
        btPersistentManifold* manifold = dynamics_world->getDispatcher()->getManifoldByIndexInternal(i);
        const btCollisionObject* body_0 = static_cast<const btCollisionObject*>(manifold->getBody0());
        const btCollisionObject* body_1 = static_cast<const btCollisionObject*>(manifold->getBody1());

        BulletRigidBodyNode* node_0 = find_body_node_from_bt_object(body_0);
        BulletRigidBodyNode* node_1 = find_body_node_from_bt_object(body_1);
        if (!(node_0 && node_1)) {
            continue;
        }

        if (node_0->is_contact_checking_enabled()) {
            node_0->on_contact(*node_1);
        }
        if (node_1->is_contact_checking_enabled()) {
            node_1->on_contact(*node_0);
        }
    }
}

void BulletPhysicsServer::register_rigid_body(BulletRigidBodyNode* rigid_body_node) {
    dynamics_world->addRigidBody(rigid_body_node->get_bt_rigid_body());
    rigid_bodies.push_back(rigid_body_node);
}

void BulletPhysicsServer::unregister_rigid_body(BulletRigidBodyNode* rigid_body_node) {
    const auto iter = std::find(
        rigid_bodies.begin(), rigid_bodies.end(),
        rigid_body_node
    );
    rigid_bodies.erase(iter);

    dynamics_world->removeCollisionObject(rigid_body_node->get_bt_rigid_body());
}

void BulletPhysicsServer::set_gravity(const glm::vec3& gravity) {
    dynamics_world->setGravity(glm_vec3_to_bullet(gravity));
}

[[nodiscard]] glm::vec3 BulletPhysicsServer::get_gravity() const {
    return bullet_vec3_to_glm<float>(dynamics_world->getGravity());
}

[[nodiscard]] std::optional<glm::vec3> BulletPhysicsServer::raycast_closest(const glm::vec3& from, const glm::vec3& to) const {
    btVector3 bt_from = glm_vec3_to_bullet(from);
    btVector3 bt_to = glm_vec3_to_bullet(to);

    btCollisionWorld::ClosestRayResultCallback result(bt_from, bt_to);
    result.m_flags |= btTriangleRaycastCallback::kF_FilterBackfaces;

    dynamics_world->rayTest(bt_from, bt_to, result);

    if (!result.hasHit()) {
        return std::nullopt;
    }

    btVector3 bt_point = bt_from.lerp(bt_to, result.m_closestHitFraction);
    return bullet_vec3_to_glm<float>(bt_point);
}

[[nodiscard]] llengine::BulletRigidBodyNode*
BulletPhysicsServer::find_body_node_from_bt_object(const btCollisionObject* bt_object) const {
    const auto iter = std::find_if(
        rigid_bodies.begin(),
        rigid_bodies.end(),
        [&bt_object] (llengine::BulletRigidBodyNode* node) -> bool {
            return static_cast<btCollisionObject*>(node->get_bt_rigid_body()) == bt_object;
        }
    );

    if (iter == rigid_bodies.end()) {
        return nullptr;
    }

    return *iter;
}