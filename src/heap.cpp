// Elaboração do Heap da JVM
#include "heap.hpp"
#include "object.hpp"
#include <stdexcept>

Heap::Heap() : _memoryPool(1024 * 1024, 0), _nextId(1) {} // Inicializa com 1 MB

Heap::~Heap() {}

Heap& Heap::getInstance() {
    static Heap instance;
    return instance;
}

size_t Heap::addObject(Object* object) {
    size_t id = _nextId++;
    _objectMap[id] = object;
    return id;
}

Object* Heap::getObjectById(size_t id) {
    return _objectMap.count(id) ? _objectMap[id] : nullptr;
}

void* Heap::allocate(size_t size) {
    for (size_t i = 0; i < _memoryPool.size(); i += size) {
        if (_memoryPool[i] == 0) {
            std::fill_n(&_memoryPool[i], size, 1);
            return &_memoryPool[i];
        }
    }
    throw std::runtime_error("Heap está cheio");
}

//void Heap::deallocate(void* ptr) {
//    auto offset = static_cast<char*>(ptr) - _memoryPool.data();
//    if (offset >= 0 && offset < _memoryPool.size()) {
//        std::fill_n(&_memoryPool[offset], 1, 0);
//    }
//}

void Heap::limparHeap() {
    for (auto& obj : _objectMap) {
        delete obj.second;  // Libera cada objeto armazenado
    }
    _objectMap.clear();  // Limpa o mapa de referências
}
