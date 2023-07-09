#pragma once

#include "nodes/Node.hpp"

#include <memory>

namespace llengine {
template<typename T>
concept node_concept = std::is_base_of_v<Node, T>;

/**
 * @brief Tries to convert unique_ptr of one node type to another.
 * 
 * @tparam T Type this function is converting to.
 * @tparam U Type this function is converting from.
 * @param from unique_ptr we are casting and which will be moved from on successful cast.
 * @return std::unique_ptr<T> Casted unique_ptr of the same object or nullptr if cast failed.
 */
template<node_concept T, node_concept U>
std::unique_ptr<T> node_cast(std::unique_ptr<U>&& from) {
    if (T* to_ptr = dynamic_cast<T*>(from.get())) {
        std::unique_ptr<T> result(to_ptr);
        static_cast<void>(from.release());
        return result;
    }
    else {
        return nullptr;
    }
}

/**
 * @brief Tries to convert unique_ptr of one node type to another.
 * 
 * @tparam T Type this function is converting to.
 * @tparam U Type this function is converting from.
 * @param from unique_ptr we are casting and which will be moved from on successful cast.
 * @throws std::bad_cast if the cast is impossible.
 * @return std::unique_ptr<T> Casted unique_ptr of the same object.
 */
template<node_concept T, node_concept U>
std::unique_ptr<T> throwing_node_cast(std::unique_ptr<U>&& from) {
    T& to_ref = dynamic_cast<T&>(*from);
    std::unique_ptr<T> result(&to_ref);
    static_cast<void>(from.release());
    return result;
}
}