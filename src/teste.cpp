#include <iostream>
#include "JVMStack.hpp"
#include "interpretador.cpp"

// Protótipos das funções do interpretador
std::vector<BytecodeInstruction> loadBytecode(const std::string& filename);
void executeBytecode(const std::vector<BytecodeInstruction>& instructions, JVMStack& jvmStack);

int main() {
    JVMStack jvmStack;

    // Carrega as instruções do arquivo bytecode.txt
    std::vector<BytecodeInstruction> instructions = loadBytecode("bytecode.txt");

    if (instructions.empty()) {
        std::cerr << "[ERRO] Nenhuma instrução carregada. Verifique o arquivo bytecode.txt!" << std::endl;
        return 1;
    }

    std::cout << "[INFO] Executando bytecode..." << std::endl;

    // Executa o interpretador
    executeBytecode(instructions, jvmStack);

    return 0;
}
