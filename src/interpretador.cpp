#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include "JVMStack.hpp"

// Definição de códigos de operação (Opcode)
enum OpCode {
    ILOAD,
    IADD,
    ISUB,
    IMUL,
    IDIV,
    IRETURN,
    IF_EQ,
    GOTO,
    INVALID
};

// Mapeia strings do arquivo para os opcodes
std::unordered_map<std::string, OpCode> opcodeMap = {
    {"iload", ILOAD},
    {"iadd", IADD},
    {"isub", ISUB},
    {"imul", IMUL},
    {"idiv", IDIV},
    {"ireturn", IRETURN},
    {"ifeq", IF_EQ},
    {"goto", GOTO}
};

// Estrutura para armazenar instruções bytecode
struct BytecodeInstruction {
    OpCode opcode;
    int operand;
    std::string label;
};

// Função para ler o arquivo de bytecode
std::vector<BytecodeInstruction> loadBytecode(const std::string& filename) {
    std::ifstream file(filename);
    std::vector<BytecodeInstruction> instructions;

    if (!file.is_open()) {
        std::cerr << "[ERRO] Não foi possível abrir o arquivo!" << std::endl;
        return instructions;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string operation;
        iss >> operation;

        BytecodeInstruction instruction;
        instruction.opcode = opcodeMap.count(operation) ? opcodeMap[operation] : INVALID;

        if (instruction.opcode == INVALID) {
            if (operation.back() == ':') {
                instruction.label = operation.substr(0, operation.size() - 1);
            } else {
                std::cerr << "[ERRO] Instrução desconhecida: " << operation << std::endl;
            }
        } else if (instruction.opcode == ILOAD || instruction.opcode == IF_EQ || instruction.opcode == GOTO) {
            iss >> instruction.operand; // Lê o argumento (valor ou índice do label)
        }

        instructions.push_back(instruction);
    }
    file.close();
    return instructions;
}

// Executa as instruções bytecode usando switch-case
void executeBytecode(const std::vector<BytecodeInstruction>& instructions, JVMStack& jvmStack) {
    Frame* frame = jvmStack.pushFrame(10, nullptr, 0, -1, true);
    size_t pc = 0; // Program Counter

    while (pc < instructions.size()) {
        const BytecodeInstruction& instr = instructions[pc];

        switch (instr.opcode) {
            case ILOAD:
                frame->pushOperand(instr.operand);
                break;

            case IADD: {
                if (frame->operandStackEmpty()) {
                    std::cerr << "[ERRO] Pilha vazia para iadd!" << std::endl;
                    return;
                }
                uint32_t v1 = frame->popOperand();
                uint32_t v2 = frame->popOperand();
                frame->pushOperand(v1 + v2);
                break;
            }

            case ISUB: {
                if (frame->operandStackEmpty()) {
                    std::cerr << "[ERRO] Pilha vazia para isub!" << std::endl;
                    return;
                }
                uint32_t v1 = frame->popOperand();
                uint32_t v2 = frame->popOperand();
                frame->pushOperand(v2 - v1);
                break;
            }

            case IMUL: {
                if (frame->operandStackEmpty()) {
                    std::cerr << "[ERRO] Pilha vazia para imul!" << std::endl;
                    return;
                }
                uint32_t v1 = frame->popOperand();
                uint32_t v2 = frame->popOperand();
                frame->pushOperand(v1 * v2);
                break;
            }

            case IDIV: {
                if (frame->operandStackEmpty()) {
                    std::cerr << "[ERRO] Pilha vazia para idiv!" << std::endl;
                    return;
                }
                uint32_t v1 = frame->popOperand();
                uint32_t v2 = frame->popOperand();
                if (v1 == 0) {
                    std::cerr << "[ERRO] Divisão por zero!" << std::endl;
                    return;
                }
                frame->pushOperand(v2 / v1);
                break;
            }

            case IF_EQ:
                if (frame->operandStackEmpty()) {
                    std::cerr << "[ERRO] Pilha vazia para ifeq!" << std::endl;
                    return;
                }
                if (frame->popOperand() == 0) {
                    pc = instr.operand - 1; // Ajusta para o índice do label
                }
                break;

            case GOTO:
                pc = instr.operand - 1;
                break;

            case IRETURN:
                if (frame->operandStackEmpty()) {
                    std::cerr << "[ERRO] Pilha vazia para retorno!" << std::endl;
                    return;
                }
                std::cout << "Resultado final: " << frame->popOperand() << std::endl;
                return;

            case INVALID:
                break;
        }

        pc++; // Avança para a próxima instrução
    }
}
