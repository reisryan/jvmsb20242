#include "arrayobject.h"

#include <cassert>
#include <iostream>
#include <cstdlib>

ArrayObject::ArrayObject(ValueType type, uint32_t length) 
    : _arrayType(type), _arraylength(length) {
    _elements.reserve(length); // Reservar espaço para o número inicial de elementos
}

ArrayObject::~ArrayObject() {
    // Nenhuma ação específica necessária para destruição
}

ObjectType ArrayObject::objectType() {
    return ObjectType::ARRAY;
}

ValueType ArrayObject::arrayContentType() {
    return _arrayType;
}

void ArrayObject::pushValue(Value value) {
    assert(value.type == _arrayType);
    
    _elements.push_back(value);
    _arraylength = _elements.size(); // Atualiza o arraylength
}

Value ArrayObject::removeAt(uint32_t index) {
    assert(_elements.size() > 0);
    assert(index < _elements.size());
    
    Value value = _elements[index];
    _elements.erase(_elements.begin() + index);
    _arraylength = _elements.size(); // Atualiza o arraylength
    return value;
}

Value ArrayObject::removeLast() {
    assert(_elements.size() > 0);
    
    Value value = _elements.back();
    _elements.pop_back();
    _arraylength = _elements.size(); // Atualiza o arraylength
    return value;
}

Value ArrayObject::removeFirst() {
    assert(_elements.size() > 0);
    
    Value value = _elements.front();
    _elements.erase(_elements.begin());
    _arraylength = _elements.size(); // Atualiza o arraylength
    return value;
}

uint32_t ArrayObject::getSize() {
    return _elements.size();
}

uint32_t ArrayObject::getArrayLength() {
    return _arraylength;
}

Value ArrayObject::getValue(uint32_t index) {
    assert(_elements.size() > 0);
//    assert(index < _elements.size());
    
    if (index >= _elements.size()) {
        std::cerr << "ArrayIndexOutOfBoundsException" << std::endl;
        exit(1);
    }
    
    return _elements[index];
}

void ArrayObject::changeValueAt(uint32_t index, Value value) {
    assert(index < _elements.size());
    _elements[index] = value;
}
