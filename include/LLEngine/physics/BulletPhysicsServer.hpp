#pragma once

#include <glm/vec3.hpp>

#include <memory>
#include <vector>
#include <optional>

class btBroadphaseInterface;
class btCollisionDispatcher;
class btDiscreteDynamicsWorld;
class btDefaultCollisionConfiguration;
class btSequentialImpulseConstraintSolver;
class btCollisionObject;

namespace llengine {
class BulletRigidBodyNode;

class BulletPhysicsServer {
public:
    BulletPhysicsServer();
    ~BulletPhysicsServer();
    BulletPhysicsServer(const BulletPhysicsServer& other) = delete;
    BulletPhysicsServer(BulletPhysicsServer&& other) = delete;
    BulletPhysicsServer& operator=(const BulletPhysicsServer& other) = delete;
    BulletPhysicsServer& operator=(BulletPhysicsServer&& other) = delete;

    void do_step(float delta_time);
    /**
     * @brief Must be called by every BulletDynamicBodyNode upon its creation.
     *
     * Does nothing if argument is nullptr.
     * @param rigid_body_node rigid body node that's going to be destroyed
     * or already destroyed.
     */
    void register_rigid_body(BulletRigidBodyNode* rigid_body_node);

    /**
     * @brief Must be called by every BulletDynamicBodyNode upon its destruction.
     *
     * Does nothing if argument is nullptr.
     * @param rigid_body_node rigid body node that's going to be destroyed
     * or already destroyed.
     */
    void unregister_rigid_body(BulletRigidBodyNode* rigid_body_node);

    void set_gravity(const glm::vec3& gravity);
    [[nodiscard]] glm::vec3 get_gravity() const;

    [[nodiscard]] std::optional<glm::vec3> raycast_closest(const glm::vec3& from, const glm::vec3& to) const;

private:
    std::unique_ptr<btBroadphaseInterface> broadphase_interface;
    std::unique_ptr<btCollisionDispatcher> collision_dispatcher;
    std::unique_ptr<btDefaultCollisionConfiguration> collision_configuration;
    std::unique_ptr<btSequentialImpulseConstraintSolver> impulse_constraint_solver;
    std::unique_ptr<btDiscreteDynamicsWorld> dynamics_world;

    std::vector<BulletRigidBodyNode*> rigid_bodies;

    [[nodiscard]] BulletRigidBodyNode* find_body_node_from_bt_object(const btCollisionObject* bt_object) const;
    void check_for_contacts() const;
};
}