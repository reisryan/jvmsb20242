// Elaboração do Heap da JVM
#include <string>
#include <vector>
#include <unordered_map>

class Object;

class Heap {
    private:
        Heap();
        ~Heap();
        std::vector<char> _memoryPool;
        std::unordered_map<size_t, Object*> _objMap;
        size_t _nextId;
};