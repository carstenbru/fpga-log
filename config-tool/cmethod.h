#ifndef CMETHOD_H
#define CMETHOD_H

#include <string>
#include <list>
#include "cparameter.h"

class CMethod
{
public:
    CMethod(std::string name, CParameter returnType);
    ~CMethod();

    void addParameter(CParameter paramter) { parameters.push_back(paramter); }
    bool sameSignature(CMethod& compare);

    std::string getName() { return name; }
    std::list<CParameter>* getParameters() { return &parameters; }
private:
    std::string name;

    CParameter returnType;
    std::list<CParameter> parameters;
};

#endif // CMETHOD_H
