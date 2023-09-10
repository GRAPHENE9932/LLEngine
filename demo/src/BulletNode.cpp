#include "BulletNode.hpp"
#include "node_registration.hpp"

#include <fmt/format.h>
#include <logger.hpp>

BulletNode::BulletNode() {
    this->set_collision_checking_enabled(true);
}

void BulletNode::on_contact(llengine::BulletRigidBodyNode& collided_body) {
    if (!deleted && get_parent()) {
        deleted = true;
        get_parent()->queue_remove_child(this);
    }
}

std::unique_ptr<llengine::Node> BulletNode::copy() const {
    std::unique_ptr<BulletNode> result {std::make_unique<BulletNode>()};
    copy_to(*result);
    return result;
}