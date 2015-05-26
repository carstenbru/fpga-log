#ifndef CMETHOD_H
#define CMETHOD_H

#include <string>
#include <list>
#include "cparameter.h"

class CObject;

class CMethod
{
public:
    CMethod(std::string completeName, std::string name, CParameter returnType, std::string headerFile, std::string description);
    CMethod(std::string name, CParameter returnType) : CMethod("", name, returnType, "", "") {}
    CMethod(QXmlStreamReader& in, CMethod *currentSignature, CObject* object);
    ~CMethod();

    void addParameter(CParameter paramter) { parameters.push_back(paramter); }
    bool sameSignature(CMethod& compare);

    std::string getName() { return name; }
    std::string getCompleteName() { return completeName; }
    std::list<CParameter>* getParameters() { return &parameters; }
    std::list<CParameter*> getMethodParameterPointers();
    CParameter* getParameter(std::string name);
    std::string getHeaderName() { return headerFile; }
    std::string getDescription() { return description; }

    void setHideFromUser(bool hide) { hideFromUser = hide; }
    bool getHideFromUser() { return hideFromUser; }

    friend QXmlStreamWriter& operator<<(QXmlStreamWriter& out, CMethod& cMethod);
private:
    bool setParameter(CParameter& newValue);

    std::string completeName;

    std::string name;
    std::string headerFile;
    std::string description;

    CParameter returnType;
    std::list<CParameter> parameters;

    bool hideFromUser;
};

#endif // CMETHOD_H
