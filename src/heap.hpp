#ifndef HEAP_HPP
#define HEAP_HPP

#include <vector>
#include <unordered_map>
#include <string>

class Object; // Objetos em Java

class Heap {
public:
    static Heap& getInstance(); // Singleton

    void* allocate(size_t size);          // Aloca um bloco de memória
    void limparHeap();                    // Libera todos os objetos do Heap
    Object* getObjectById(size_t id);     // Retorna objeto pelo ID
    size_t addObject(Object* object);     // Adiciona objeto ao heap

private:
    Heap();
    ~Heap();

    std::vector<char> _memoryPool;        // Bloco de memória
    std::unordered_map<size_t, Object*> _objectMap; // Mapa de IDs para objetos
    size_t _nextId;                       // ID incremental para objetos
};

#endif // HEAP_HPP
