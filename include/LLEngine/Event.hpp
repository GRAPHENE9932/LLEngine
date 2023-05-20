#pragma once

#include <cstdint>
#include <utility>
#include <algorithm>
#include <functional>

namespace llengine {
template<typename... Args>
class Event {
public:
    using id_type = std::uint32_t;
    using function_type = std::function<void(Args...)>;

    /**
     * @brief Subscribe to the event. The specified
     * function will be invoked when the event is
     * invoked.
     * 
     * @return id_type With this ID you can unsubscribe later.
     * You can ignore it if and ONLY if the event object
     * will last shorter than the specified function.
     */
    id_type subscribe(const function_type& function) {
        id_type cur_id = id_count++;

        functions.push_back(std::make_pair(
            cur_id,
            function
        ));

        return cur_id;
    }

    /**
     * @brief Unsubscribe from the event.
     * 
     * @param function_id ID you have got from subscribe method.
     * @return true Unsubscribe successful.
     * @return false Unsubscribe unsuccessful and no action will be done.
     */
    bool unsubscribe(id_type function_id) {
        const auto iter = std::find_if(
            functions.begin(), functions.end(),
            [&function_id] (const auto pair) {
                return pair.first == function_id;
            }
        );

        if (iter == functions.end()) {
            return false;
        }

        functions.erase(iter);
        return true;
    }

    void invoke() const {
        for (const auto cur_pair : functions) {
            cur_pair.second();
        }
    }

    void operator()() const {
        invoke();
    }

private:
    static inline id_type id_count {0};
    std::vector<std::pair<id_type, function_type>> functions;
};
}