#pragma once

#include <glm/vec3.hpp>

#include <memory>
#include <chrono>
#include <vector>

class btBroadphaseInterface;
class btCollisionDispatcher;
class btDiscreteDynamicsWorld;
class btDefaultCollisionConfiguration;
class btSequentialImpulseConstraintSolver;

class BulletRigidBodyNode;

class BulletPhysicsServer {
public:
    ~BulletPhysicsServer();
    BulletPhysicsServer(const BulletPhysicsServer& other) = delete;
    void operator=(const BulletPhysicsServer& other) = delete;

    static BulletPhysicsServer& get_instance() {
        static BulletPhysicsServer instance;
        singleton_initialized = true;
        return instance;
    }

    static void do_step_if_needed(float delta_time);

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

private:
    static inline bool singleton_initialized {false};

    std::unique_ptr<btDefaultCollisionConfiguration> collision_configuration;
    std::unique_ptr<btCollisionDispatcher> collision_dispatcher;
    std::unique_ptr<btBroadphaseInterface> broadphase_interface;
    std::unique_ptr<btSequentialImpulseConstraintSolver> impulse_constraint_solver;
    std::unique_ptr<btDiscreteDynamicsWorld> dynamics_world;

    std::vector<BulletRigidBodyNode*> rigid_bodies;

    BulletPhysicsServer();
};
