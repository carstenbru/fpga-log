#ifndef CMETHOD_H
#define CMETHOD_H

#include <string>
#include <list>

class DataType;

class CMethodParameter {
public:
    CMethodParameter(std::string name, DataType* dataType, bool pointer);

    bool sameSignature(CMethodParameter& compare);
    std::string getName() { return name; }
    DataType* getDataType() { return dataType; }
private:
    std::string name;

    DataType* dataType;
    bool pointer;
};

class CMethod
{
public:
    CMethod(std::string name, CMethodParameter *returnType);
    ~CMethod();

    void addParameter(CMethodParameter* paramter) { parameters.push_back(paramter); }
    bool sameSignature(CMethod& compare);

    std::string getName() { return name; }
    std::list<CMethodParameter*> getParameters() { return parameters; }
private:
    std::string name;

    CMethodParameter* returnType;
    std::list<CMethodParameter*> parameters;
};

#endif // CMETHOD_H
