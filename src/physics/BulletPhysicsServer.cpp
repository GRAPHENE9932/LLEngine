#include "physics/BulletPhysicsServer.hpp"
#include "utils/glm_bullet_conversion.hpp"
#include "nodes/physics/BulletRigidBodyNode.hpp"

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

    dynamics_world->stepSimulation(delta_time);
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
