#ifndef CMETHOD_H
#define CMETHOD_H

#include <string>
#include <list>
#include "cparameter.h"

class CMethod
{
public:
    CMethod(std::string name, CParameter returnType, std::string headerFile);
    CMethod(std::string name, CParameter returnType) : CMethod(name, returnType, "") {}
    CMethod(QXmlStreamReader& in);
    ~CMethod();

    void addParameter(CParameter paramter) { parameters.push_back(paramter); }
    bool sameSignature(CMethod& compare);

    std::string getName() { return name; }
    std::list<CParameter>* getParameters() { return &parameters; }
    CParameter* getParameter(std::string name);
    std::string getHeaderName() { return headerFile; }

    friend QXmlStreamWriter& operator<<(QXmlStreamWriter& out, CMethod& cMethod);
private:
    std::string name;
    std::string headerFile;

    CParameter returnType;
    std::list<CParameter> parameters;
};

#endif // CMETHOD_H
