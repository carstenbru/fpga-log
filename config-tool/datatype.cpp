#include "datatype.h"

std::map<std::string, DataType*> DataType::types;

DataType::DataType(std::string name, bool systemType) :
    name(name),
    systemType(systemType),
    super(NULL)
{
    types[name] = this;
}

std::string DataType::getCleanedName() {
    std::string s = name;
    size_t i = s.rfind("_t");
    if (i < s.length())
        s.erase(i, s.length());
    return s;
}
