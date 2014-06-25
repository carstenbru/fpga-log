#ifndef DATATYPE_H
#define DATATYPE_H

#include <string>
#include <list>
#include <map>
#include "cmethod.h"

class DataType
{
public:
    DataType(std::string name, bool systemType);

    std::string getName() { return name; }
    std::string getCleanedName();
    std::string getDisplayName() { return getCleanedName(); }
    DataType* getSuperType() { return super; }
    std::list<DataType*> getChilds() { return childs; }
    bool hasPrefix(std::string prefix) { return (name.find(prefix) == 0); }
    bool isAbstract();
    bool isSystemType() { return systemType; }
    std::list<CMethod*> getMethods() { return methods; }

    void setSuperType(DataType* super) { this->super = super; }
    void addChild(DataType* child) { childs.push_back(child); }
    void addMethod(CMethod* method) { methods.push_back(method); }

    static DataType* getType(std::string name) { return types.at(name); }
    static std::map<std::string, DataType*> getTypes() { return types; }
private:
    std::string name;
    bool systemType;
    DataType* super;

    std::list<DataType*> childs;
    std::list<CMethod*> methods;

    static std::map<std::string, DataType*> types;
};

#endif // DATATYPE_H
