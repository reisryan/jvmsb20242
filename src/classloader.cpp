#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <memory>
#include <stdint.h>

// Definição de tipos básicos
typedef uint8_t u1;
typedef uint16_t u2;
typedef uint32_t u4;
typedef uint64_t u8;

// Estrutura ClassFile
struct ClassFile {
    u4 magic;
    u2 minor_version;
    u2 major_version;
    u2 constant_pool_count;
    // Outros campos serão adicionados
};

// Leitura em big-endian
u2 readU2(std::ifstream &file) {
    u2 value = 0;
    value |= file.get() << 8;
    value |= file.get();
    return value;
}

u4 readU4(std::ifstream &file) {
    u4 value = 0;
    value |= file.get() << 24;
    value |= file.get() << 16;
    value |= file.get() << 8;
    value |= file.get();
    return value;
}

// Leitura do arquivo .class
void readClassFile(const std::string &filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file) {
        std::cerr << "Erro ao abrir o arquivo: " << filePath << std::endl;
        return;
    }

    ClassFile cf;
    cf.magic = readU4(file);

    // Verifica o Magic Number
    if (cf.magic != 0xCAFEBABE) {
        std::cerr << "O arquivo não é um arquivo .class válido." << std::endl;
        return;
    }
    std::cout << "Magic Number: 0xCAFEBABE\n";

    // Lê versão do arquivo
    cf.minor_version = readU2(file);
    cf.major_version = readU2(file);
    std::cout << "Versão menor: " << cf.minor_version << ", Versão maior: " << cf.major_version << "\n";

    // Lê o pool de constantes
    cf.constant_pool_count = readU2(file);
    std::cout << "Tamanho do Constant Pool: " << cf.constant_pool_count - 1 << "\n";

    // Continuação do parsing (como leitura do pool de constantes)
    for (u2 i = 1; i < cf.constant_pool_count; i++) {
        u1 tag = file.get();
        std::cout << "Entrada " << i << " do Constant Pool, Tag: " << static_cast<int>(tag) << "\n";
    }

    file.close();
}

int main() {
    const std::string filePath = "MeuArquivo.class"; // Substitua pelo caminho do seu arquivo
    readClassFile(filePath);
    return 0;
}