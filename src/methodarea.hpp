#ifndef METHOD_AREA_HPP
#define METHOD_AREA_HPP

#include <unordered_map>
#include <vector>
#include <string>
#include <stdexcept>

struct ConstantPoolEntry;
struct method_info;
struct field_info_entry;

class MethodArea {
public:
    static MethodArea& getInstance(); // Singleton

    void loadClass(const std::string& className, const std::vector<ConstantPoolEntry>& pool,
                   const std::vector<method_info>& methods, const std::vector<field_info_entry>& fields);

    const std::vector<ConstantPoolEntry>& getConstantPool(const std::string& className);
    const std::vector<method_info>& getMethods(const std::string& className);
    const std::vector<field_info_entry>& getFields(const std::string& className);

private:
    MethodArea() {} // Construtor privado para Singleton
    ~MethodArea() {}

    std::unordered_map<std::string, std::vector<ConstantPoolEntry>> classConstantPools;
    std::unordered_map<std::string, std::vector<method_info>> classMethods;
    std::unordered_map<std::string, std::vector<field_info_entry>> classFields;
};

#endif // METHOD_AREA_HPP
