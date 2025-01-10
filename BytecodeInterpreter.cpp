#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>
#include <unordered_map>
#include <cstdint>

std::vector<std::string> pool_strings; // Vetor de strings

// Função auxiliar para ler dados do arquivo
template<typename T>

void readData(std::ifstream &file, T &data) {
    file.read(reinterpret_cast<char*>(&data), sizeof(T));
}

// Função para ler uma string UTF-8 do pool de constantes
std::string readUtf8(std::ifstream &file) {
    uint16_t length;
    readData(file, length);
    length = ((length >> 8) & 0xFF) | ((length << 8) & 0xFF00);
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
        //uint16_t length;
        //std::variant<std::vector<char>, int> buffer;
        //std::vector<char> buffer;
        struct {
            uint16_t nameIndex;
            uint16_t descriptorIndex;
        } nameAndType;
    } info;
};

struct attribute_info {
    uint16_t attribute_name_index;
    uint64_t attribute_length = 0;
    //uint8_t info [attribute_length];
    std::vector<uint8_t> info; // Use um vetor dinâmico
};

struct field_info_entry {
    uint16_t access_flags;
    uint16_t name_index;
    uint16_t descriptor_index;
    uint16_t attributes_count = 0;
    //attribute_info attributes[attributes_count];
    std::vector<attribute_info> attributes; // Use um vetor dinâmico
};

// Função para ler o pool de constantes
void readConstantPool(std::ifstream &file, std::vector<ConstantPoolEntry> &constantPool) {
    uint16_t x = 0;
    uint16_t constantPoolCount;
    readData(file, constantPoolCount);
    constantPoolCount = ((constantPoolCount >> 8) & 0xFF) | ((constantPoolCount << 8) & 0xFF00);
    constantPool.resize(constantPoolCount);
    std::cout << "Constant pool count: " << constantPoolCount  << std::endl;

    for (int i = 1; i < constantPoolCount; i++) {
        ConstantPoolEntry &entry = constantPool[i];
        readData(file, entry.tag);
        std::cout << "tag: " << (int)entry.tag  << std::endl;

        switch (entry.tag) {
            case 7: // CONSTANT_Class
                readData(file, entry.info.nameIndex);
                entry.info.nameIndex = ((entry.info.nameIndex >> 8) & 0xFF) | ((entry.info.nameIndex << 8) & 0xFF00);
                break;

            case 9: // CONSTANT_Fieldref
            case 10: // CONSTANT_Methodref
            case 11: // CONSTANT_InterfaceMethodref
                readData(file, entry.info.ref.classIndex);
                readData(file, entry.info.ref.nameAndTypeIndex);
                entry.info.ref.classIndex = ((entry.info.ref.classIndex >> 8) & 0xFF) | ((entry.info.ref.classIndex << 8) & 0xFF00);
                entry.info.ref.nameAndTypeIndex = ((entry.info.ref.nameAndTypeIndex >> 8) & 0xFF) | ((entry.info.ref.nameAndTypeIndex << 8) & 0xFF00);
                break;

            case 8: // CONSTANT_String
                readData(file, entry.info.stringIndex);
                entry.info.stringIndex = ((entry.info.stringIndex >> 8) & 0xFF) | ((entry.info.stringIndex << 8) & 0xFF00);
                break;

            case 3: // CONSTANT_Integer
                readData(file, entry.info.intValue);
                entry.info.intValue = ((entry.info.intValue >> 24) & 0xFF) | ((entry.info.intValue >> 8) & 0xFF00) | ((entry.info.intValue << 8) & 0xFF0000) | ((entry.info.intValue << 24) & 0xFF000000);
                break;

            case 4: // CONSTANT_Float
                readData(file, entry.info.floatValue);
                entry.info.floatValue = (float)((((uint32_t)entry.info.floatValue >> 24) & 0xFF) | (((uint32_t)entry.info.floatValue >> 8) & 0xFF00) | (((uint32_t)entry.info.floatValue << 8) & 0xFF0000) | (((uint32_t)entry.info.floatValue << 24) & 0xFF000000));
                break;

            case 5: // CONSTANT_Long
                readData(file, entry.info.longValue);
                entry.info.longValue = ((entry.info.longValue >> 56) & 0xFF) |
                                       ((entry.info.longValue >> 40) & 0xFF00) |
                                       ((entry.info.longValue >> 24) & 0xFF0000) |
                                       ((entry.info.longValue >> 8)  & 0xFF000000) |
                                       ((entry.info.longValue << 8)  & 0xFF00000000) |
                                       ((entry.info.longValue << 24) & 0xFF0000000000) |
                                       ((entry.info.longValue << 40) & 0xFF000000000000) |
                                       ((entry.info.longValue << 56) & 0xFF00000000000000);

                ++i; // Long e double ocupam dois índices no pool de constantes
                break;

            case 6: // CONSTANT_Double
                readData(file, entry.info.doubleValue);
                entry.info.doubleValue = (double)((((uint64_t)entry.info.doubleValue >> 56) & 0xFF) |
                                         (((uint64_t)entry.info.doubleValue >> 40) & 0xFF00) |
                                         (((uint64_t)entry.info.doubleValue >> 24) & 0xFF0000) |
                                         (((uint64_t)entry.info.doubleValue >> 8)  & 0xFF000000) |
                                         (((uint64_t)entry.info.doubleValue << 8)  & 0xFF00000000) |
                                         (((uint64_t)entry.info.doubleValue << 24) & 0xFF0000000000) |
                                         (((uint64_t)entry.info.doubleValue << 40) & 0xFF000000000000) |
                                         (((uint64_t)entry.info.doubleValue << 56) & 0xFF00000000000000));
                ++i; // Long e double ocupam dois índices no pool de constantes
                break;

            case 12: // CONSTANT_NameAndType
                readData(file, entry.info.nameAndType.nameIndex);
                readData(file, entry.info.nameAndType.descriptorIndex);
                entry.info.nameAndType.nameIndex = ((entry.info.nameAndType.nameIndex >> 8) & 0xFF) | ((entry.info.nameAndType.nameIndex << 8) & 0xFF00);
                entry.info.nameAndType.descriptorIndex = ((entry.info.nameAndType.descriptorIndex >> 8) & 0xFF) | ((entry.info.nameAndType.descriptorIndex << 8) & 0xFF00);
                break;

            case 1: // CONSTANT_Utf8
                // Lê a string UTF-8 diretamente do arquivo
                pool_strings.push_back(readUtf8(file));
                /*readData(file, entry.info.length);
                entry.info.length = ((entry.info.length >> 8) & 0xFF) | ((entry.info.length << 8) & 0xFF00);
                entry.info.buffer.resize(entry.info.length);
                file.read(entry.info.buffer.data(), entry.info.length);*/
                entry.info.nameIndex = x; // Não usado, placeholder
                x++;
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
                /*std::cout << "Class name: ";
                for (int i = 0; i < entry.info.length; ++i) {
                    std::cout << "Class name: " << entry.info.buffer[i];
                }
                std::cout << std::endl;*/
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
                std::cout << "Utf8\t\t" << std::endl; // Placeholder
                //std::string stringUTF8 = readUtf8(file);
                std::cout << "Length of byte array: " << pool_strings[size_t(entry.info.nameIndex)].size() << std::endl;
                std::cout << "String: " << pool_strings[size_t(entry.info.nameIndex)] << std::endl;
                break;

            default:
                std::cout << "Unknown" << std::endl;
                break;
        }
    }
}

// Função para ler as flags de acesso
void displayAcessFlag (std::ifstream &file, uint16_t acess_flag) {
    std::cout << "Nome do flag: ";
        switch (acess_flag) {
            case 1:
                std::cout << "ACC_PUBLIC " << "(" << "0x" << std::hex << acess_flag << std::dec << ")" << std::endl;
                break;

            case 16:
                std::cout << "ACC_FINAL " << "(" << "0x" << std::hex << acess_flag << std::dec << ")" << std::endl;
                break;

            case 32:
                std::cout << "ACC_SUPER " << "(" << "0x" << std::hex << acess_flag << std::dec << ")" << std::endl;
                break;

            case 512:
                std::cout << "ACC_INTERFACE " << "(" << "0x" << std::hex << acess_flag << std::dec << ")" << std::endl;
                break;

            case 1024:
                std::cout << "ACC_ABSTRACT " << "(" << "0x" << std::hex << acess_flag << std::dec << ")" << std::endl;
                break;

            case 4096:
                std::cout << "ACC_SYNTHETIC " << "(" << "0x" << std::hex << acess_flag << std::dec << ")" << std::endl;
                break;

            case 8192:
                std::cout << "ACC_ANNOTATION " << "(" << "0x" << std::hex << acess_flag << std::dec << ")" << std::endl;
                break;

            case 16384:
                std::cout << "ACC_ENUM " << "(" << "0x" << std::hex << acess_flag << std::dec << ")" << std::endl;
                break;

            default:
                throw std::runtime_error("Flag de acesso desconhecida: " + std::to_string(acess_flag));
        }
}

// Função para ler as interfaces implementadas pela classe ou interface do arquivo

void readinterfaces(std::ifstream &file, std::vector<uint16_t> &interfaces) {
    uint16_t interfaces_count;
    readData(file, interfaces_count);
    interfaces_count = ((interfaces_count >> 8) & 0xFF) | ((interfaces_count << 8) & 0xFF00);
    interfaces.resize(interfaces_count);
    std::cout << "Interfaces count: " << interfaces_count  << std::endl;

    for (int i = 0; i < interfaces_count; i++) {
        uint16_t &entry = interfaces[i];
        readData(file, entry);
        entry = ((entry >> 8) & 0xFF) | ((entry << 8) & 0xFF00);
    }
}

// Função para exibir as interfaces implementadas pela classe ou interface do arquivo

void displayinterfaces(const std::vector<uint16_t> &interfaces, const std::vector<ConstantPoolEntry> &constantPool) {
    for (size_t i = 0; i < interfaces.size(); ++i) {
        const uint16_t &entry = interfaces[i];
        std::cout << "Interface:  cp_info #" << interfaces[i] << "   <" << pool_strings[size_t(constantPool[size_t(constantPool[size_t(interfaces[i])].info.nameIndex)].info.nameIndex)] << ">" << std::endl;
    }
}



// Função para ler as variáveis de classe ou variáveis de instância

void readfieldinfo(std::ifstream &file, std::vector<field_info_entry> &field_info) {
    uint16_t fields_count;
    readData(file, fields_count);
    fields_count = ((fields_count >> 8) & 0xFF) | ((fields_count << 8) & 0xFF00);

    std::cout << "Fields count count: " << fields_count  << std::endl;
}

// Função para exibir as variáveis de classe ou variáveis de instância

void displayfield_info(const std::vector<field_info_entry> &field_info) {

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
    readData(file, magic);   // Leitura feita em little endian order

    // Verificar se é necessário inverter a ordem dos bytes
    magic = ((magic >> 24) & 0xFF) | ((magic >> 8) & 0xFF00) | ((magic << 8) & 0xFF0000) | ((magic << 24) & 0xFF000000);   // Máscara para obter dado em big endian order

    std::cout << "Número mágico lido: " << std::hex << magic << std::dec << std::endl;

    if (magic != 0xCAFEBABE) {
        std::cerr << "Arquivo .class inválido" << std::endl;
        return 1;
    }

    // Ler versões
    uint16_t minorVersion, majorVersion;
    readData(file, minorVersion);
    readData(file, majorVersion);
    majorVersion = ((majorVersion >> 8) & 0xFF) | ((majorVersion << 8) & 0xFF00);
    std::cout << "Versão: " << majorVersion << "." << minorVersion << std::endl;

    // Ler e exibir o pool de constantes
    std::vector<ConstantPoolEntry> constantPool;
    readConstantPool(file, constantPool);
    displayConstantPool(constantPool);

    // Outros elementos do arquivo .class podem ser lidos e exibidos de forma similar...

    // Ler e exibir permissões de acesso da classe ou interface
    uint16_t Acess_Flag;        // Flags de acesso
    readData(file, Acess_Flag);
    Acess_Flag = ((Acess_Flag >> 8) & 0xFF) | ((Acess_Flag << 8) & 0xFF00);
    displayAcessFlag(file, Acess_Flag);

    // Ler e exibir índice que indica o nome da classe ou interface definida por esse arquivo
    uint16_t this_class;        // Índice na pool de constantes
    uint16_t aux_ind;
    readData(file, this_class);
    this_class = ((this_class >> 8) & 0xFF) | ((this_class << 8) & 0xFF00);
    aux_ind = constantPool[size_t(this_class)].info.nameIndex;
    std::cout << "This class:  cp_info #" << this_class << "   <" << pool_strings[constantPool[size_t(aux_ind)].info.nameIndex] << ">" << std::endl;

    // Ler e exibir índice que indica o nome da super classe direta (classe mãe) da classe definida nesse arquivo
    uint16_t super_class;        // Índice na pool de constantes
    readData(file, super_class);
    super_class = ((super_class >> 8) & 0xFF) | ((super_class << 8) & 0xFF00);
    aux_ind = constantPool[size_t(super_class)].info.nameIndex;
    std::cout << "Super class: cp_info #" << super_class << "   <" << pool_strings[constantPool[size_t(aux_ind)].info.nameIndex] << ">" << std::endl;

    // Ler e exibir o pool de constantes
    std::vector<uint16_t> interfaces;
    readinterfaces(file, interfaces);
    displayinterfaces(interfaces, constantPool);

    // Ler e exibir as variáveis de classe ou variáveis de instância
    std::vector<field_info_entry> field_info;
    readfieldinfo(file, field_info);
    displayfield_info(field_info);



    return 0;
}
