#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>
#include <unordered_map>

// Função auxiliar para ler dados do arquivo
template<typename T>
void readData(std::ifstream &file, T &data) {
    file.read(reinterpret_cast<char*>(&data), sizeof(T));
}

// Função para ler uma string UTF-8 do pool de constantes
std::string readUtf8(std::ifstream &file) {
    uint16_t length;
    readData(file, length);
    std::vector<char> buffer(length);
    file.read(buffer.data(), length);
    return std::string(buffer.begin(), buffer.end());
}

// Estrutura para armazenar entradas do pool de constantes
struct ConstantPoolEntry {
    uint8_t tag;
    union {
        uint16_t nameIndex;
        struct {
            uint16_t classIndex;
            uint16_t nameAndTypeIndex;
        } ref;
        uint32_t intValue;
        float floatValue;
        uint64_t longValue;
        double doubleValue;
        uint16_t stringIndex;
        struct {
            uint16_t nameIndex;
            uint16_t descriptorIndex;
        } nameAndType;
    } info;
};

// Função para ler o pool de constantes
void readConstantPool(std::ifstream &file, std::vector<ConstantPoolEntry> &constantPool) {
    uint16_t constantPoolCount;
    readData(file, constantPoolCount);
    constantPool.resize(constantPoolCount);
    
    for (int i = 1; i < constantPoolCount; ++i) {
        ConstantPoolEntry &entry = constantPool[i];
        readData(file, entry.tag);
        
        switch (entry.tag) {
            case 7: // CONSTANT_Class
                readData(file, entry.info.nameIndex);
                break;
                
            case 9: // CONSTANT_Fieldref
            case 10: // CONSTANT_Methodref
            case 11: // CONSTANT_InterfaceMethodref
                readData(file, entry.info.ref.classIndex);
                readData(file, entry.info.ref.nameAndTypeIndex);
                break;
                
            case 8: // CONSTANT_String
                readData(file, entry.info.stringIndex);
                break;
                
            case 3: // CONSTANT_Integer
                readData(file, entry.info.intValue);
                break;
                
            case 4: // CONSTANT_Float
                readData(file, entry.info.floatValue);
                break;
                
            case 5: // CONSTANT_Long
                readData(file, entry.info.longValue);
                ++i; // Long e double ocupam dois índices no pool de constantes
                break;
                
            case 6: // CONSTANT_Double
                readData(file, entry.info.doubleValue);
                ++i; // Long e double ocupam dois índices no pool de constantes
                break;
                
            case 12: // CONSTANT_NameAndType
                readData(file, entry.info.nameAndType.nameIndex);
                readData(file, entry.info.nameAndType.descriptorIndex);
                break;
                
            case 1: // CONSTANT_Utf8
                // Lê a string UTF-8 diretamente do arquivo
                entry.info.nameIndex = 0; // Não usado, placeholder
                break;
                
            default:
                throw std::runtime_error("Tag desconhecida no pool de constantes: " + std::to_string(entry.tag));
        }
    }
}

// Função para exibir o pool de constantes
void displayConstantPool(const std::vector<ConstantPoolEntry> &constantPool) {
    for (size_t i = 1; i < constantPool.size(); ++i) {
        const ConstantPoolEntry &entry = constantPool[i];
        std::cout << "#" << i << " = ";
        
        switch (entry.tag) {
            case 7:
                std::cout << "Class\t\t#" << entry.info.nameIndex << std::endl;
                break;
                
            case 9:
                std::cout << "Fieldref\t#" << entry.info.ref.classIndex << ".#" << entry.info.ref.nameAndTypeIndex << std::endl;
                break;
                
            case 10:
                std::cout << "Methodref\t#" << entry.info.ref.classIndex << ".#" << entry.info.ref.nameAndTypeIndex << std::endl;
                break;
                
            case 11:
                std::cout << "InterfaceMethodref\t#" << entry.info.ref.classIndex << ".#" << entry.info.ref.nameAndTypeIndex << std::endl;
                break;
                
            case 8:
                std::cout << "String\t\t#" << entry.info.stringIndex << std::endl;
                break;
                
            case 3:
                std::cout << "Integer\t\t" << entry.info.intValue << std::endl;
                break;
                
            case 4:
                std::cout << "Float\t\t" << entry.info.floatValue << std::endl;
                break;
                
            case 5:
                std::cout << "Long\t\t" << entry.info.longValue << std::endl;
                break;
                
            case 6:
                std::cout << "Double\t\t" << entry.info.doubleValue << std::endl;
                break;
                
            case 12:
                std::cout << "NameAndType\t#" << entry.info.nameAndType.nameIndex << ":#" << entry.info.nameAndType.descriptorIndex << std::endl;
                break;
                
            case 1:
                std::cout << "Utf8\t\t" << entry.info.nameIndex << std::endl; // Placeholder
                break;
                
            default:
                std::cout << "Unknown" << std::endl;
                break;
        }
    }
}

// Função principal para ler e exibir um arquivo .class
int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Uso: " << argv[0] << " <arquivo.class>" << std::endl;
        return 1;
    }

    std::ifstream file(argv[1], std::ios::binary);
    if (!file) {
        std::cerr << "Erro ao abrir o arquivo: " << argv[1] << std::endl;
        return 1;
    }

    // Ler e verificar o número mágico
    uint32_t magic;
    readData(file, magic);
    if (magic != 0xCAFEBABE) {
        std::cerr << "Arquivo .class inválido" << std::endl;
        return 1;
    }

    // Ler versões
    uint16_t minorVersion, majorVersion;
    readData(file, minorVersion);
    readData(file, majorVersion);
    std::cout << "Versão: " << majorVersion << "." << minorVersion << std::endl;

    // Ler e exibir o pool de constantes
    std::vector<ConstantPoolEntry> constantPool;
    readConstantPool(file, constantPool);
    displayConstantPool(constantPool);

    // Outros elementos do arquivo .class podem ser lidos e exibidos de forma similar...

    return 0;
}
