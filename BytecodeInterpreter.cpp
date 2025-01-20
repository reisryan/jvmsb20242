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

// struct attribute_info {
//     uint16_t attribute_name_index;
//     uint64_t attribute_length = 0;
//     //uint8_t info [attribute_length];
//     std::vector<uint8_t> info; // Use um vetor dinâmico
// };

struct attribute_info {
    uint16_t attribute_name_index;
    uint32_t attribute_length;  // Mudado para uint32_t para corresponder ao formato do arquivo .class
    std::vector<uint8_t> info;
};

struct field_info_entry {
    uint16_t access_flags;
    uint16_t name_index;
    uint16_t descriptor_index;
    uint16_t attributes_count = 0;
    //attribute_info attributes[attributes_count];
    std::vector<attribute_info> attributes; // Use um vetor dinâmico
};

//A estrutura method_info descreve um método da classe
struct method_info {
    uint16_t access_flags;
    uint16_t name_index;
    uint16_t descriptor_index;
    uint16_t attributes_count;
    std::vector<attribute_info> attributes;
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
        //std::cout << "tag: " << (int)entry.tag  << std::endl;

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
    uint16_t x;
    std::cout << std::endl;
    std::cout << "POOL DE CONSTANTES" << std::endl;
    for (size_t i = 1; i < constantPool.size(); ++i) {
        const ConstantPoolEntry &entry = constantPool[i];
        const ConstantPoolEntry &entry2 = constantPool[i];
        std::cout << "[" << i << "]";
        switch (entry.tag) {
            case 7:
                std::cout << " CONSTANT_Class_info:" << std::endl;
                std::cout << "  Class name: cp_info #" << entry.info.nameIndex << "     <" << pool_strings[constantPool[entry.info.nameIndex].info.nameIndex] << ">" << std::endl;
                std::cout << std::endl;
                break;
            case 9:
                std::cout << " CONSTANT_Fieldref_info:" << std::endl;
                std::cout << "  Class name: cp_info#" << entry.info.ref.classIndex << "     <" << pool_strings[constantPool[constantPool[entry.info.ref.classIndex].info.nameIndex].info.nameIndex] << ">" << std::endl;
                std::cout << "  Name and type: cp_info#" << entry.info.ref.nameAndTypeIndex << "    <" << pool_strings[constantPool[constantPool[entry.info.ref.nameAndTypeIndex].info.nameIndex].info.nameIndex] << ">" << std::endl;
                std::cout << std::endl;
                break;

            case 10:
                std::cout << " CONSTANT_Methodref_info:" << std::endl;
                std::cout << "  Class name: cp_info#" << entry.info.ref.classIndex << "     <" << pool_strings[constantPool[constantPool[entry.info.ref.classIndex].info.nameIndex].info.nameIndex] << ">" << std::endl;
                std::cout << "  Name and type: cp_info#" << entry.info.ref.nameAndTypeIndex << "     <" << pool_strings[constantPool[constantPool[entry.info.ref.nameAndTypeIndex].info.nameIndex].info.nameIndex] << ">" << std::endl;
                std::cout << std::endl;
                /*std::cout << "Class name: ";
                for (int i = 0; i < entry.info.length; ++i) {
                    std::cout << "Class name: " << entry.info.buffer[i];
                }
                std::cout << std::endl;*/
                break;

            case 11:
                std::cout << " CONSTANT_InterfaceMethodref_info:" << std::endl;
                std::cout << "  Class name: cp_info#" << entry.info.ref.classIndex << "     <" << pool_strings[constantPool[constantPool[entry.info.ref.classIndex].info.nameIndex].info.nameIndex] << ">" << std::endl;
                std::cout << "  Name and type: cp_info#" << entry.info.ref.nameAndTypeIndex << "     <" << pool_strings[constantPool[constantPool[entry.info.ref.nameAndTypeIndex].info.nameIndex].info.nameIndex] << ">" << std::endl;
                std::cout << std::endl;
                break;

            case 8:
                std::cout << " CONSTANT_String_info:" << std::endl;
                std::cout << "  String: cp_info#" << entry.info.stringIndex << "    <" << pool_strings[constantPool[entry.info.stringIndex].info.nameIndex] << ">" << std::endl;
                std::cout << std::endl;
                break;

            case 3:
                std::cout << " CONSTANT_Intenger_info:" << std::endl;
                std::cout << "  Integer: " << entry.info.intValue << std::endl;
                std::cout << std::endl;
                break;

            case 4:
                std::cout << " CONSTANT_Float_info:" << std::endl;
                std::cout << "  Float: " << entry.info.floatValue << std::endl;
                break;

            case 5:
                std::cout << "CONSTANT_Long_info:" << std::endl;
                std::cout << "  Long: " << entry.info.longValue << std::endl;
                std::cout << std::endl;
                break;

            case 6:
                std::cout << " CONSTANT_Double_info:" << std::endl;
                std::cout << "  Double: " << entry.info.doubleValue << std::endl;
                std::cout << std::endl;
                break;

            case 12:
                std::cout << " CONSTANT_NameAndType_info:" << std::endl;
                std::cout << "  Name: cp_info#" << entry.info.nameAndType.nameIndex << "     <" << pool_strings[constantPool[entry.info.nameAndType.nameIndex].info.nameIndex] << ">" << std::endl;
                std::cout << "  Descriptor: cp_info#" << entry.info.nameAndType.descriptorIndex << "     <" << pool_strings[constantPool[entry.info.nameAndType.descriptorIndex].info.nameIndex] << ">" << std::endl;
                std::cout << std::endl;
                break;

            case 1:
                std::cout << " CONSTANT_Utf8_info:" << std::endl;
                //std::string stringUTF8 = readUtf8(file);
                std::cout << "  Length of byte array: " << pool_strings[size_t(entry.info.nameIndex)].size() << std::endl;
                std::cout << "  String: " << pool_strings[size_t(entry.info.nameIndex)] << std::endl;
                std::cout << std::endl;
                break;

            default:
                std::cout << "Unknown" << std::endl;
                std::cout << std::endl;
                break;
        }
    }
}

// Função para ler as flags de acesso
void displayAcessFlag (std::ifstream &file, uint16_t acess_flag) {
    std::cout << "      Nome da flag: ";
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


// atualizado :



void readAttribute(std::ifstream &file, attribute_info &attribute) {
    readData(file, attribute.attribute_name_index);
    attribute.attribute_name_index = ((attribute.attribute_name_index >> 8) & 0xFF) |
                                   ((attribute.attribute_name_index << 8) & 0xFF00);

    uint32_t length;
    readData(file, length);
    // Correção na conversão do comprimento do atributo
    attribute.attribute_length = ((length >> 24) & 0xFF) |
                               ((length >> 8) & 0xFF00) |
                               ((length << 8) & 0xFF0000) |
                               ((length << 24) & 0xFF000000);

    // Redimensionar o vetor info antes de ler
    attribute.info.resize(attribute.attribute_length);
    if (attribute.attribute_length > 0) {
        file.read(reinterpret_cast<char*>(attribute.info.data()), attribute.attribute_length);
    }
}

// Função para ler os campos da classe (usando field_info_entry)
void readFields(std::ifstream &file, std::vector<field_info_entry> &fields) {
    uint16_t fields_count;
    readData(file, fields_count);
    fields_count = ((fields_count >> 8) & 0xFF) | ((fields_count << 8) & 0xFF00);
    fields.resize(fields_count);
    std::cout << "Fields count: " << fields_count  << std::endl;

    for (uint16_t i = 0; i < fields_count; ++i) {
        field_info_entry &field = fields[i];
        readData(file, field.access_flags);
        readData(file, field.name_index);
        readData(file, field.descriptor_index);
        readData(file, field.attributes_count);

        field.access_flags = ((field.access_flags >> 8) & 0xFF) | ((field.access_flags << 8) & 0xFF00);
        field.name_index = ((field.name_index >> 8) & 0xFF) | ((field.name_index << 8) & 0xFF00);
        field.descriptor_index = ((field.descriptor_index >> 8) & 0xFF) | ((field.descriptor_index << 8) & 0xFF00);
        field.attributes_count = ((field.attributes_count >> 8) & 0xFF) | ((field.attributes_count << 8) & 0xFF00);

        field.attributes.resize(field.attributes_count);
        for (uint16_t j = 0; j < field.attributes_count; ++j) {
            readAttribute(file, field.attributes[j]);
        }
    }
}

void displayFields(const std::vector<field_info_entry> &fields,
                   const std::vector<ConstantPoolEntry> &constantPool)
{
    for (const auto &field : fields) {
        if (field.name_index == 0 || field.name_index >= constantPool.size() ||
            field.descriptor_index == 0 || field.descriptor_index >= constantPool.size()) {
            std::cerr << "Erro: índice inválido em `constantPool` para field." << std::endl;
            continue;
        }

        const ConstantPoolEntry &nameEntry       = constantPool[field.name_index];
        const ConstantPoolEntry &descriptorEntry = constantPool[field.descriptor_index];

        if (nameEntry.tag != 1 || descriptorEntry.tag != 1) {
            std::cerr << "Erro: field name ou descriptor não são CONSTANT_Utf8." << std::endl;
            continue;
        }

        std::string fieldName = pool_strings[nameEntry.info.nameIndex];
        std::string fieldDescriptor = pool_strings[descriptorEntry.info.nameIndex];

        std::cout << "Field: " << fieldName
                  << " | Descriptor: " << fieldDescriptor << std::endl;

        // Se quiser imprimir atributos do campo:
        for (const auto &attr : field.attributes) {
            if (attr.attribute_name_index >= pool_strings.size()) {
                std::cerr << "Erro: índice de atributo inválido." << std::endl;
                continue;
            }
            std::string attributeName = pool_strings[attr.attribute_name_index];
            std::cout << "  Field Attribute: " << attributeName
                      << " (length=" << attr.attribute_length << ")\n";
        }
    }
}


void readMethods(std::ifstream &file, std::vector<method_info> &methods) {
    uint16_t methods_count;
    readData(file, methods_count);
    methods_count = ((methods_count >> 8) & 0xFF) | ((methods_count << 8) & 0xFF00);
    methods.resize(methods_count);
    std::cout << "Methods count: " << methods_count  << std::endl;
    for (uint16_t i = 0; i < methods_count; ++i) {
        method_info &method = methods[i];
        readData(file, method.access_flags);
        readData(file, method.name_index);
        readData(file, method.descriptor_index);
        readData(file, method.attributes_count);

        method.access_flags = ((method.access_flags >> 8) & 0xFF) | ((method.access_flags << 8) & 0xFF00);
        method.name_index = ((method.name_index >> 8) & 0xFF) | ((method.name_index << 8) & 0xFF00);
        method.descriptor_index = ((method.descriptor_index >> 8) & 0xFF) | ((method.descriptor_index << 8) & 0xFF00);
        method.attributes_count = ((method.attributes_count >> 8) & 0xFF) | ((method.attributes_count << 8) & 0xFF00);

        method.attributes.resize(method.attributes_count);
        for (uint16_t j = 0; j < method.attributes_count; ++j) {
            readAttribute(file, method.attributes[j]);
        }
    }
}

void displayMethods(const std::vector<method_info> &methods,
                    const std::vector<ConstantPoolEntry> &constantPool)
{   std::cout << std::endl;
    for (const auto &method : methods) {
        // 1) Verifica se os índices estão dentro do range do constantPool
        if (method.name_index == 0 || method.name_index >= constantPool.size() ||
            method.descriptor_index == 0 || method.descriptor_index >= constantPool.size()) {
            std::cerr << "Erro: índice inválido em `constantPool` para método." << std::endl;
            continue;
        }

        // 2) Acessa as entradas correspondentes do constantPool
        const ConstantPoolEntry &nameEntry       = constantPool[method.name_index];
        const ConstantPoolEntry &descriptorEntry = constantPool[method.descriptor_index];

        // 3) Verifica se são, de fato, CONSTANT_Utf8
        if (nameEntry.tag != 1 || descriptorEntry.tag != 1) {
            std::cerr << "Erro: name_index ou descriptor_index não apontam para CONSTANT_Utf8." << std::endl;
            continue;
        }

        // 4) Agora sim pegamos o índice interno que aponta para o `pool_strings`.
        //    Esse índice fica em nameEntry.info.nameIndex.
        //    Então obtemos as strings de fato:
        std::string methodName      = pool_strings[nameEntry.info.nameIndex];
        std::string methodDescriptor = pool_strings[descriptorEntry.info.nameIndex];

        std::cout << "Method: " << methodName
                  << " Descriptor: " << methodDescriptor << std::endl;

        // Exibe atributos do método
        for (const auto &attribute : method.attributes) {
            // Verifica índice do atributo no constantPool
            if (attribute.attribute_name_index == 0 ||
                attribute.attribute_name_index >= pool_strings.size()) {
                std::cerr << "Erro: índice inválido em `pool_strings` para atributo." << std::endl;
                continue;
            }

            // Nome do atributo
            std::string attributeName = pool_strings[attribute.attribute_name_index];
            std::cout << "  Attribute: " << attributeName
                      << " Length: " << attribute.attribute_length << std::endl;

            // Exemplo de processamento caso seja "Code"
            if (attributeName == "Code") {
                if (attribute.info.size() < 8) {
                    std::cerr << "Erro: tamanho insuficiente de `info` no atributo 'Code'." << std::endl;
                    continue;
                }
                // Ler max_stack, max_locals, code_length, etc.
                uint16_t maxStack  = (attribute.info[0] << 8) | attribute.info[1];
                uint16_t maxLocals = (attribute.info[2] << 8) | attribute.info[3];
                uint32_t codeLength = (attribute.info[4] << 24) | (attribute.info[5] << 16) |
                                      (attribute.info[6] << 8)  | attribute.info[7];

                std::cout << "    Max Stack: "   << maxStack   << std::endl;
                std::cout << "    Max Locals: "  << maxLocals  << std::endl;
                std::cout << "    Code Length: " << codeLength << std::endl;

                // Verificar se realmente tem `codeLength` bytes de bytecode
                if (attribute.info.size() < (8 + codeLength)) {
                    std::cerr << "Erro: tamanho insuficiente de `info` para o bytecode." << std::endl;
                    continue;
                }

                // Exibir o bytecode
                std::cout << "    Bytecode: ";
                for (uint32_t i = 0; i < codeLength; ++i) {
                    std::cout << std::hex << (int)attribute.info[8 + i] << " ";
                }
                std::cout << std::dec << std::endl;
            }
        }
    }
}


void interpretAttributes(const attribute_info &attribute, const std::vector<ConstantPoolEntry> &constantPool) {
    std::string attributeName = pool_strings[attribute.attribute_name_index];

    if (attributeName == "Code") {
        std::cout << "  Code attribute found." << std::endl;
        // Decodifique o bytecode aqui
    } else if (attributeName == "LineNumberTable") {
        std::cout << "  LineNumberTable attribute found." << std::endl;
        // Decodifique os dados aqui
    } else {
        std::cout << "  Unknown attribute: " << attributeName << std::endl;
    }
}

void displayClassAttributes(const std::vector<attribute_info> &attributes,
                          const std::vector<ConstantPoolEntry> &constantPool) {
    std::cout << "\n=== Class Attributes ===\n";
    for (const auto &attribute : attributes) {
        // Verificar se o índice é válido
        if (attribute.attribute_name_index >= constantPool.size()) {
            std::cerr << "Invalid attribute name index\n";
            continue;
        }

        const auto &nameEntry = constantPool[attribute.attribute_name_index];
        if (nameEntry.tag != 1) {
            std::cerr << "Invalid attribute entry type\n";
            continue;
        }

        if (nameEntry.info.nameIndex >= pool_strings.size()) {
            std::cerr << "Invalid string pool index\n";
            continue;
        }

        std::string attributeName = pool_strings[nameEntry.info.nameIndex];
        std::cout << "Class Attribute: " << attributeName
                  << " Length: " << attribute.attribute_length << "\n";

        if (attributeName == "SourceFile" && attribute.info.size() >= 2) {
            uint16_t sourceFileIndex = (attribute.info[0] << 8) | attribute.info[1];
            if (sourceFileIndex < pool_strings.size()) {
                std::cout << "  Source File: " << pool_strings[sourceFileIndex] << "\n";
            }
        }
    }
}





// Funcao de auxilio para veerificar erro no acesse flag

void handleAccessFlags(std::ifstream &file, uint16_t access_flag) {
    // Desmembrar as combinações de flags
    if (access_flag & 0x0001) { // ACC_PUBLIC
        displayAcessFlag(file, 0x0001);
    }
    if (access_flag & 0x0010) { // ACC_FINAL
        displayAcessFlag(file, 0x0010);
    }
    if (access_flag & 0x0020) { // ACC_SUPER
        displayAcessFlag(file, 0x0020);
    }
    if (access_flag & 0x0200) { // ACC_INTERFACE
        displayAcessFlag(file, 0x0200);
    }
    if (access_flag & 0x0400) { // ACC_ABSTRACT
        displayAcessFlag(file, 0x0400);
    }
    if (access_flag & 0x1000) { // ACC_SYNTHETIC
        displayAcessFlag(file, 0x1000);
    }
    if (access_flag & 0x2000) { // ACC_ANNOTATION
        displayAcessFlag(file, 0x2000);
    }
    if (access_flag & 0x4000) { // ACC_ENUM
        displayAcessFlag(file, 0x4000);
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
    //displayConstantPool(constantPool);

    // Outros elementos do arquivo .class podem ser lidos e exibidos de forma similar...

    // Ler e exibir permissões de acesso da classe ou interface
    uint16_t Acess_Flag;        // Flags de acesso
    readData(file, Acess_Flag);
    Acess_Flag = ((Acess_Flag >> 8) & 0xFF) | ((Acess_Flag << 8) & 0xFF00);
    std::cout << "Flags de acesso: " << std::endl;
    //displayAcessFlag(file, Acess_Flag);
    handleAccessFlags(file, Acess_Flag);

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
    readFields(file, field_info);
    //displayFields(field_info, constantPool);

     // Ler e exibir os métodos
     std::vector<method_info> methods;
     readMethods(file, methods);
     //displayMethods(methods, constantPool);

     // Ler e exibir os atributos da classe
    uint16_t attributes_count;
    readData(file, attributes_count);
    attributes_count = ((attributes_count >> 8) & 0xFF) | ((attributes_count << 8) & 0xFF00);
    std::cout << "Attributes count: " << attributes_count  << std::endl;

     std::vector<attribute_info> attributes(attributes_count);
     for (uint16_t i = 0; i < attributes_count; ++i) {
         readAttribute(file, attributes[i]);
     }
     //displayClassAttributes(attributes, constantPool);

    displayConstantPool(constantPool);
    displayFields(field_info, constantPool);
    displayMethods(methods, constantPool);
    displayClassAttributes(attributes, constantPool);
    return 0;
}
