#ifndef FRAME_HPP
#define FRAME_HPP

#include <vector>
#include <iostream>
#include <cstdint>
#include "./operandstack.hpp" // Corrigido o nome do arquivo para seguir o padrão

/**
 * @brief Representa um frame de execução na JVM, contendo:
 * - Vetor de variáveis locais
 * - Pilha de operandos
 * - Pool de constantes
 * - Endereço de retorno
 */
class Frame {
public:
    std::vector<uint32_t> localVariables; ///< Vetor de variáveis locais (32 bits por slot)
    OperandStack operandStack;            ///< Pilha de operandos
    void* constantPool;                    ///< Ponteiro para o pool de constantes
    uint32_t returnAddress;                ///< Endereço de retorno para continuar execução

    /**
     * @brief Constrói um frame com tamanho fixo para variáveis locais e a pilha de operandos.
     * 
     * @param maxLocals Número de slots de variáveis locais.
     * @param maxStack Tamanho máximo da pilha de operandos.
     * @param constantPool Ponteiro para o pool de constantes.
     */
    Frame(int maxLocals, int maxStack, void* constantPool);

    // Métodos para variáveis locais (32 bits)
    void setLocalVariable(int index, uint32_t value);
    uint32_t getLocalVariable(int index);

    // Métodos para variáveis locais (64 bits)
    void setLongOrDouble(int index, uint64_t value);
    uint64_t getLongOrDouble(int index);

    // Métodos para manipular a pilha de operandos
    void pushOperand(uint32_t value);
    uint32_t popOperand();
    void pushLongOperand(int64_t value);
    int64_t popLongOperand();
    void pushDoubleOperand(double value);
    double popDoubleOperand();

    // Métodos para gerenciar o endereço de retorno
    void setReturnAddress(uint32_t address);
    uint32_t getReturnAddress();
};

#endif // FRAME_HPP