#include "object.hpp"
#include <sstream>

// Construtor: gera um ID único
Object::Object() : objectId(0) {}

Object::~Object() {}

size_t Object::getObjectId() const {
    return objectId;
}

// Implementação do equals: compara IDs dos objetos
bool Object::equals(const Object* other) const {
    if (other == nullptr) return false;
    return this->objectId == other->objectId;
}

// Implementação do toString: retorna uma string representando o objeto
std::string Object::toString() const {
    std::ostringstream oss;
    oss << "Object@" << this;
    return oss.str();
}
