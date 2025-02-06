#include <iostream>
#include "./framestack.hpp"

Frame::Frame(int maxLocals, int maxStack, void* constantPool)
    : localVariables(maxLocals, 0), operandStack(maxStack), constantPool(constantPool), returnAddress(0) {}

// ======= MÉTODOS PARA VARIÁVEIS LOCAIS =======

void Frame::setLocalVariable(int index, uint32_t value) {
    if (index < 0 || index >= static_cast<int>(localVariables.size())) {
        std::cerr << "[ERRO] Acesso inválido a localVariables[" << index << "]!" << std::endl;
        exit(1);
    }
    localVariables[index] = value;
}

uint32_t Frame::getLocalVariable(int index) {
    if (index < 0 || index >= static_cast<int>(localVariables.size())) {
        std::cerr << "[ERRO] Acesso inválido a localVariables[" << index << "]!" << std::endl;
        exit(1);
    }
    return localVariables[index];
}

void Frame::setLongOrDouble(int index, uint64_t value) {
    if (index < 0 || index + 1 >= static_cast<int>(localVariables.size())) {
        std::cerr << "[ERRO] Tentativa de armazenar long/double fora dos limites!" << std::endl;
        exit(1);
    }
    localVariables[index] = static_cast<uint32_t>(value >> 32); // Parte alta (big-endian)
    localVariables[index + 1] = static_cast<uint32_t>(value);   // Parte baixa
}

uint64_t Frame::getLongOrDouble(int index) {
    if (index < 0 || index + 1 >= static_cast<int>(localVariables.size())) {
        std::cerr << "[ERRO] Tentativa de acessar long/double fora dos limites!" << std::endl;
        exit(1);
    }
    return (static_cast<uint64_t>(localVariables[index]) << 32) | localVariables[index + 1];
}

// ======= MÉTODOS PARA PILHA DE OPERANDOS =======

void Frame::pushOperand(uint32_t value) {
    operandStack.pushInt(value);
}

uint32_t Frame::popOperand() {
    return operandStack.popInt();
}

void Frame::pushLongOperand(int64_t value) {
    operandStack.pushLong(value);
}

int64_t Frame::popLongOperand() {
    return operandStack.popLong();
}

void Frame::pushDoubleOperand(double value) {
    operandStack.pushDouble(value);
}

double Frame::popDoubleOperand() {
    return operandStack.popDouble();
}

// ======= MÉTODOS PARA GERENCIAR O ENDEREÇO DE RETORNO =======

void Frame::setReturnAddress(uint32_t address) {
    returnAddress = address;
}

uint32_t Frame::getReturnAddress() {
    return returnAddress;
}
