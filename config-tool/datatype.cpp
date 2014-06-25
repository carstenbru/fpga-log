#include "datatype.h"

using namespace std;

std::map<std::string, DataType*> DataType::types;

DataType::DataType(std::string name) :
    name(name),
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

bool DataType::isAbstract() {
    for (list<CMethod*>::iterator i = methods.begin(); i != methods.end(); i++) {
        if ((*i)->getName().compare("init") == 0)
            return false;
    }
    return true;
}
