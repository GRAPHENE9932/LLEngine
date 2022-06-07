#include "Rect.hpp"

bool Rect::operator==(const Rect& other) const {
    return this->position == other.position && this->size == other.size;
}