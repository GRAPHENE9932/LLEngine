#pragma once

#include <physics/BulletPhysicsServer.hpp>
#include <nodes/physics/BulletRigidBodyNode.hpp>

class BulletNode : public llengine::BulletRigidBodyNode {
public:
    void start() final override;

    void on_contact(llengine::BulletRigidBodyNode& collided_body) final override;
    std::unique_ptr<llengine::Node> copy() const final override;

private:
    bool deleted = false;
};