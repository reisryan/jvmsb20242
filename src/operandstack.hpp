#ifndef OPERAND_STACK_HPP
#define OPERAND_STACK_HPP

#include <cstdint>
#include <vector>
#include <cstring>
#include <stdexcept>

/**
 * @brief Union que representa um slot de 32 bits.
 * 
 * Pode armazenar um inteiro, um float ou uma referência (void*).
 */
union Slot {
    int32_t num;  ///< Usado para inteiros ou para cada parte de um valor de 64 bits
    float   f;    ///< Usado para armazenar um float
    void*   ref;  ///< Usado para armazenar uma referência a objeto
};

/**
 * @brief Classe que implementa a pilha de operandos de um frame da JVM.
 * 
 * A pilha possui tamanho fixo (determinado em tempo de compilação)
 * e é criada vazia. Operações _push_ e _pop_ são fornecidas para os
 * tipos de 32 bits (cat1) e para os tipos de 64 bits (cat2) que utilizam
 * dois _slots_ (armazenados em big-endian order).
 */
class OperandStack {
public:
    /**
     * @brief Constrói uma pilha de operandos com tamanho máximo de _slots_.
     * 
     * @param maxSize Número máximo de slots da pilha.
     */
    explicit OperandStack(size_t maxSize);

    // Operações para tipos "cat1" (ocupam 1 slot)
    void pushInt(int32_t value);
    int32_t popInt();

    void pushFloat(float value);
    float popFloat();

    void pushRef(void* ref);
    void* popRef();

    // Operações para tipos "cat2" (ocupam 2 slots)
    void pushLong(int64_t value);
    int64_t popLong();

    void pushDouble(double value);
    double popDouble();

    /// Retorna se a pilha está vazia.
    bool isEmpty() const;

    /// Retorna o número de slots atualmente ocupados na pilha.
    size_t size() const;

private:
    std::vector<Slot> slots;  ///< Vetor de slots que compõe a pilha.
    size_t sp;                ///< Stack pointer (próximo índice livre).

    /// Verifica se há espaço para empilhar _numSlots_ slots.
    void checkPush(size_t numSlots);
    /// Verifica se há _numSlots_ slots para desempilhar.
    void checkPop(size_t numSlots);
};

#endif // OPERAND_STACK_HPP