#include "RectangularGUINode.hpp"

class CompleteRectangularGUINode : public RectangularGUINode {
public:
    void set_size(glm::vec2 new_size) {
        size = new_size;
    }

    [[nodiscard]] glm::vec2 get_size() const override {
        return size;
    }

private:
    glm::vec2 size;
};