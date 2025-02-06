#ifndef JVM_STACK_HPP
#define JVM_STACK_HPP

#include <stack>
#include <iostream>
#include "./framestack.hpp" // Inclui a definição da classe Frame

/**
 * @brief Representa a pilha de execução da JVM, contendo múltiplos frames.
 */
class JVMStack {
private:
    std::stack<Frame*> stack; ///< Pilha de frames

public:
    Frame* pushFrame(int maxLocals, int thisObject = -1, bool isStatic = false);
    Frame* popFrame();
    Frame* currentFrame();
    bool isEmpty();
    ~JVMStack();
};

#endif // JVM_STACK_HPP
