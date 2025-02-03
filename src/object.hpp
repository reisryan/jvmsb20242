#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <string>

class Object {
private:
    size_t objectId; // Identificador único do objeto

public:
    Object();
    virtual ~Object();

    size_t getObjectId() const; // Retorna o ID do objeto

    virtual bool equals(const Object* other) const; // Compara dois objetos
    virtual std::string toString() const; // Representação em string do objeto
};

#endif // OBJECT_HPP
