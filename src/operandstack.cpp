#include "./operandstack.hpp"

OperandStack::OperandStack(size_t maxSize)
    : slots(maxSize), sp(0)
{
}

void OperandStack::checkPush(size_t numSlots) {
    if (sp + numSlots > slots.size()) {
        throw std::out_of_range("OperandStack overflow: não há espaço para empilhar valor(es).");
    }
}

void OperandStack::checkPop(size_t numSlots) {
    if (sp < numSlots) {
        throw std::out_of_range("OperandStack underflow: não há valor(es) suficiente(s) para desempilhar.");
    }
}

// ===== Operações para tipos "cat1" (1 slot) =====

void OperandStack::pushInt(int32_t value) {
    checkPush(1);
    slots[sp].num = value;
    sp++;
}

int32_t OperandStack::popInt() {
    checkPop(1);
    sp--;
    return slots[sp].num;
}

void OperandStack::pushFloat(float value) {
    checkPush(1);
    slots[sp].f = value;
    sp++;
}

float OperandStack::popFloat() {
    checkPop(1);
    sp--;
    return slots[sp].f;
}

void OperandStack::pushRef(void* ref) {
    checkPush(1);
    slots[sp].ref = ref;
    sp++;
}

void* OperandStack::popRef() {
    checkPop(1);
    sp--;
    return slots[sp].ref;
}

// ===== Operações para tipos "cat2" (2 slots) =====

void OperandStack::pushLong(int64_t value) {
    checkPush(2);
    // Armazena em ordem big-endian: primeiro os 32 bits mais significativos,
    // depois os 32 bits menos significativos.
    slots[sp].num     = static_cast<int32_t>(value >> 32);
    slots[sp + 1].num = static_cast<int32_t>(value & 0xFFFFFFFFLL);
    sp += 2;
}

int64_t OperandStack::popLong() {
    checkPop(2);
    sp -= 2;
    int64_t high = static_cast<uint32_t>(slots[sp].num);
    int64_t low  = static_cast<uint32_t>(slots[sp + 1].num);
    return (high << 32) | low;
}

void OperandStack::pushDouble(double value) {
    checkPush(2);
    uint64_t bits;
    std::memcpy(&bits, &value, sizeof(bits)); // Converte o double para representação binária
    slots[sp].num     = static_cast<int32_t>(bits >> 32);
    slots[sp + 1].num = static_cast<int32_t>(bits & 0xFFFFFFFFULL);
    sp += 2;
}

double OperandStack::popDouble() {
    checkPop(2);
    sp -= 2;
    uint64_t high = static_cast<uint32_t>(slots[sp].num);
    uint64_t low  = static_cast<uint32_t>(slots[sp + 1].num);
    uint64_t bits = (high << 32) | low;
    double value;
    std::memcpy(&value, &bits, sizeof(value)); // Converte os bits de volta para double
    return value;
}

bool OperandStack::isEmpty() const {
    return sp == 0;
}

size_t OperandStack::size() const {
    return sp;
}
