#include "MethodArea.hpp"
#include <iostream>

MethodArea& MethodArea::getInstance() {
    static MethodArea instance;
    return instance;
}

void MethodArea::loadClass(const std::string& className, const std::vector<ConstantPoolEntry>& pool,
                           const std::vector<method_info>& methods, const std::vector<field_info_entry>& fields) {
    if (classConstantPools.size() > 10000) {  // Simulando um limite de memória da JVM
        throw std::runtime_error("OutOfMemoryError: Não há espaço na Área de Métodos");
    }

    classConstantPools[className] = pool;
    classMethods[className] = methods;
    classFields[className] = fields;
}

const std::vector<ConstantPoolEntry>& MethodArea::getConstantPool(const std::string& className) {
    return classConstantPools[className];
}

const std::vector<method_info>& MethodArea::getMethods(const std::string& className) {
    return classMethods[className];
}

const std::vector<field_info_entry>& MethodArea::getFields(const std::string& className) {
    return classFields[className];
}
