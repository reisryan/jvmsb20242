#include <iostream>
#include <vector>
#include <stack>
#include <unordered_map>
#include <stdexcept>
#include <cstdlib>  // Para exit()
#include <cstdint>  // Para uint32_t

class Frame {
public:
    std::vector<uint32_t> localVariables; // Vetor de variáveis locais (32 bits por slot)
    std::stack<uint32_t> operandStack;    // Pilha de operandos (32 bits por slot)

    Frame(int maxLocals) {
        localVariables.resize(maxLocals, 0); // Inicializa as variáveis locais com 0
    }

    // Armazena um valor de 32 bits em uma variável local
    void setLocalVariable(int index, uint32_t value) {
        if (index < 0 || index >= static_cast<int>(localVariables.size())) {
            std::cerr << "[ERRO] Acesso inválido a localVariables[" << index << "]!" << std::endl;
            exit(1);
        }
        localVariables[index] = value;
    }

    // Obtém um valor de 32 bits de uma variável local
    uint32_t getLocalVariable(int index) {
        if (index < 0 || index >= static_cast<int>(localVariables.size())) {
            std::cerr << "[ERRO] Acesso inválido a localVariables[" << index << "]!" << std::endl;
            exit(1);
        }
        return localVariables[index];
    }

    // Armazena um valor de 64 bits (long ou double) em dois slots consecutivos
    void setLongOrDouble(int index, uint64_t value) {
        if (index < 0 || index + 1 >= static_cast<int>(localVariables.size())) {
            std::cerr << "[ERRO] Tentativa de armazenar long/double fora dos limites!" << std::endl;
            exit(1);
        }
        localVariables[index] = static_cast<uint32_t>(value >> 32); // Parte alta (big-endian)
        localVariables[index + 1] = static_cast<uint32_t>(value);   // Parte baixa
    }

    // Obtém um valor de 64 bits (long ou double) de dois slots consecutivos
    uint64_t getLongOrDouble(int index) {
        if (index < 0 || index + 1 >= static_cast<int>(localVariables.size())) {
            std::cerr << "[ERRO] Tentativa de acessar long/double fora dos limites!" << std::endl;
            exit(1);
        }
        return (static_cast<uint64_t>(localVariables[index]) << 32) | localVariables[index + 1];
    }

    // Empilha um valor na pilha de operandos (32 bits)
    void pushOperand(uint32_t value) {
        operandStack.push(value);
    }

    // Remove e retorna um valor da pilha de operandos (32 bits)
    uint32_t popOperand() {
        if (operandStack.empty()) {
            std::cerr << "[ERRO] Tentativa de popOperand() em pilha vazia!" << std::endl;
            exit(1);
        }
        uint32_t value = operandStack.top();
        operandStack.pop();
        return value;
    }
};

class JVMStack {
private:
    std::stack<Frame*> stack; // Pilha de frames

public:
    // Criar um novo frame e adicioná-lo à pilha
    Frame* pushFrame(int maxLocals, int thisObject = -1, bool isStatic = false) {
        Frame* frame = new Frame(maxLocals);

        // Métodos de instância armazenam `this` em localVariables[0]
        if (!isStatic && thisObject != -1) {
            frame->setLocalVariable(0, static_cast<uint32_t>(thisObject));
        }

        stack.push(frame);
        return frame;
    }

    // Remove o frame atual e retorna um ponteiro para ele
    Frame* popFrame() {
        if (stack.empty()) {
            std::cerr << "[ERRO] Nenhum frame disponível para pop!" << std::endl;
            exit(1);
        }
        Frame* frame = stack.top();
        stack.pop();
        return frame;
    }

    // Retorna o frame atual sem removê-lo
    Frame* currentFrame() {
        if (stack.empty()) {
            std::cerr << "[ERRO] Tentativa de acessar frame atual em pilha vazia!" << std::endl;
            exit(1);
        }
        return stack.top();
    }

    // Verifica se a pilha está vazia
    bool isEmpty() {
        return stack.empty();
    }

    // Libera memória dos frames ao final da execução
    ~JVMStack() {
        while (!stack.empty()) {
            delete stack.top();
            stack.pop();
        }
    }
};

int main() {
    JVMStack jvmStack;

    // Criando um frame para o método "main" (método estático)
    Frame* mainFrame = jvmStack.pushFrame(2, -1, true); // Método estático

    // Criando um frame para um método de instância (this = objeto 42)
    int objectRef = 42; // Simulando um objeto na heap
    Frame* instanceMethodFrame = jvmStack.pushFrame(3, objectRef);

    // Verificando se o `this` foi armazenado corretamente
    std::cout << "Referência ao objeto (this): " << instanceMethodFrame->getLocalVariable(0) << std::endl;

    // Simulando chamada de método add(5, 3)
    Frame* addFrame = jvmStack.pushFrame(3, -1, true); // Método estático
    addFrame->setLocalVariable(0, 5); // Parâmetro a
    addFrame->setLocalVariable(1, 3); // Parâmetro b

    // Simulando execução de a + b
    uint32_t a = addFrame->getLocalVariable(0);
    uint32_t b = addFrame->getLocalVariable(1);
    addFrame->pushOperand(a + b); // Empilha o resultado

    // Simulando retorno de add() para main()
    uint32_t result = addFrame->popOperand();
    jvmStack.popFrame(); // Remove frame de add()

    // Armazena o resultado no frame main()
    mainFrame->pushOperand(result);

    // Exibe o resultado
    std::cout << "Resultado: " << mainFrame->popOperand() << std::endl;

    // Finaliza execução removendo o frame main()
    jvmStack.popFrame();

    return 0;
}
