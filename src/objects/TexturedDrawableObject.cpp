#include "TexturedDrawableObject.hpp"

TexturedDrawableObject::TexturedDrawableObject(GLuint texture_id, std::shared_ptr<Mesh> mesh) :
    texture_id(texture_id) {
    this->mesh = mesh;
}

TexturedDrawableObject::~TexturedDrawableObject() {

}
