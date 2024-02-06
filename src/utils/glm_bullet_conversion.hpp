#pragma once

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <bullet/LinearMath/btVector3.h>
#include <bullet/LinearMath/btQuaternion.h>

namespace llengine {
template<typename T>
[[nodiscard]] inline btQuaternion glm_quat_to_bullet(const glm::qua<T>& orig) {
    return btQuaternion(
        static_cast<btScalar>(orig.x),
        static_cast<btScalar>(orig.y),
        static_cast<btScalar>(orig.z),
        static_cast<btScalar>(orig.w)
    );
}

template<typename T>
[[nodiscard]] inline btVector3 glm_vec3_to_bullet(const glm::vec<3, T>& orig) {
    return btVector3(
        static_cast<btScalar>(orig.x),
        static_cast<btScalar>(orig.y),
        static_cast<btScalar>(orig.z)
    );
}

template<typename T>
[[nodiscard]] inline glm::vec3 bullet_vec3_to_glm(const btVector3& orig) {
    return glm::vec<3, T>(
        static_cast<T>(orig.getX()),
        static_cast<T>(orig.getY()),
        static_cast<T>(orig.getZ())
    );
}

template<typename T>
[[nodiscard]] inline glm::qua<T> bullet_quat_to_glm(const btQuaternion& orig) {
    return glm::qua<T>(
        static_cast<T>(orig.getW()),
        static_cast<T>(orig.getX()),
        static_cast<T>(orig.getY()),
        static_cast<T>(orig.getZ())
    );
}
}