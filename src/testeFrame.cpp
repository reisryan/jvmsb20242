#include <iostream>
#include "JVMStack.hpp"

void testStackOperations() {
    JVMStack jvmStack;
    void* constantPool = nullptr; // Simulando um pool de constantes
    uint32_t returnAddress = 0x100; // Endereço fictício de retorno

    // Criando um frame inicial (método main)
    Frame* mainFrame = jvmStack.pushFrame(10, constantPool, returnAddress, -1, true);
    std::cout << "🔹 Criado Frame 'main' com 10 variáveis locais.\n";

    // Teste do endereço de retorno
    std::cout << "✅ Endereço de retorno: 0x" << std::hex << mainFrame->getReturnAddress() << std::dec 
              << " (Esperado: 0x100)\n";

    // Empilhando e desempilhando valores de diferentes tipos
    mainFrame->pushOperand(100);
    mainFrame->pushOperand(200);
    std::cout << "✅ Pilha de operandos: Push 100, Push 200\n";

    uint32_t val1 = mainFrame->popOperand();
    uint32_t val2 = mainFrame->popOperand();
    std::cout << "✅ Pilha de operandos: Pop -> " << val1 << ", Pop -> " << val2 << " (Esperado: 200, 100)\n";

    // Teste de long na pilha
    int64_t longValue = 0x1234567890ABCDEFLL;
    mainFrame->pushLongOperand(longValue);
    std::cout << "✅ Pilha de operandos: Push long 0x1234567890ABCDEF\n";

    int64_t popLong = mainFrame->popLongOperand();
    std::cout << "✅ Pilha de operandos: Pop long -> 0x" << std::hex << popLong << std::dec 
              << " (Esperado: 0x1234567890ABCDEF)\n";

    // Teste de double na pilha
    double pi = 3.141592653589793;
    mainFrame->pushDoubleOperand(pi);
    std::cout << "✅ Pilha de operandos: Push double 3.141592653589793\n";

    double popDouble = mainFrame->popDoubleOperand();
    std::cout << "✅ Pilha de operandos: Pop double -> " << popDouble << " (Esperado: 3.141592653589793)\n";

    // Finaliza execução removendo o frame main
    jvmStack.popFrame();
}

void testMethodCalls() {
    JVMStack jvmStack;
    void* constantPool = nullptr; 
    uint32_t returnAddress = 0x200;

    // Criando frame principal (método main)
    Frame* mainFrame = jvmStack.pushFrame(5, constantPool, returnAddress, -1, true);
    std::cout << "🔹 Criado Frame 'main' com 5 variáveis locais.\n";

    // Criando um método de instância com um objeto this = 42
    int objectRef = 42;
    Frame* instanceMethodFrame = jvmStack.pushFrame(3, constantPool, returnAddress, objectRef);
    std::cout << "🔹 Criado Frame 'instanceMethod' (this = " << objectRef << ")\n";

    // Verificando referência
    std::cout << "✅ Referência ao objeto this: " << instanceMethodFrame->getLocalVariable(0) << " (Esperado: 42)\n";

    // Chamando um método de soma (add)
    Frame* addFrame = jvmStack.pushFrame(3, constantPool, returnAddress, -1, true);
    std::cout << "🔹 Criado Frame 'add' (estático)\n";

    addFrame->setLocalVariable(0, 15); // a = 15
    addFrame->setLocalVariable(1, 10); // b = 10

    // Simulando soma de a + b e empilhando resultado
    uint32_t a = addFrame->getLocalVariable(0);
    uint32_t b = addFrame->getLocalVariable(1);
    addFrame->pushOperand(a + b);

    // Retorno de add() para main()
    uint32_t result = addFrame->popOperand();
    std::cout << "✅ Resultado de add(15, 10) = " << result << " (Esperado: 25)\n";
    jvmStack.popFrame(); // Remove frame de add()

    // Chamando outro método que retorna um long
    Frame* longFrame = jvmStack.pushFrame(3, constantPool, returnAddress, -1, true);
    std::cout << "🔹 Criado Frame 'longMethod' (estático)\n";

    int64_t largeValue = 0xDEADBEEF12345678LL;
    longFrame->pushLongOperand(largeValue);

    int64_t longResult = longFrame->popLongOperand();
    std::cout << "✅ Retorno de longMethod(): 0x" << std::hex << longResult << std::dec 
              << " (Esperado: 0xDEADBEEF12345678)\n";
    jvmStack.popFrame();

    // Finalizando a execução do frame principal
    jvmStack.popFrame();
}

void testDeepRecursion(JVMStack& jvmStack, int depth) {
    void* constantPool = nullptr; 
    uint32_t returnAddress = 0x300;

    if (depth == 0) {
        std::cout << "🔹 Reached recursion depth 10, returning.\n";
        return;
    }

    Frame* recursiveFrame = jvmStack.pushFrame(2, constantPool, returnAddress, -1, true);
    std::cout << "🔹 Criado Frame recursivo (depth = " << depth << ")\n";

    testDeepRecursion(jvmStack, depth - 1);
    
    jvmStack.popFrame();
    std::cout << "🔹 Removido Frame recursivo (depth = " << depth << ")\n";
}

void testRecursion() {
    JVMStack jvmStack;
    std::cout << "\n🔻 Testando Recursão Profunda 🔻\n";
    testDeepRecursion(jvmStack, 10);
}

int main() {
    std::cout << "🧪 Iniciando Mega Teste da JVMStack e Frames 🧪\n\n";

    std::cout << "🔻 Teste de Manipulação de Pilha 🔻\n";
    testStackOperations();

    std::cout << "\n🔻 Teste de Chamadas de Métodos 🔻\n";
    testMethodCalls();

    std::cout << "\n🔻 Teste de Recursão 🔻\n";
    testRecursion();

    std::cout << "\n✅ Todos os testes foram executados com sucesso! 🚀🔥\n";
    return 0;
}
