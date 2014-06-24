#include "cmethod.h"

using namespace std;

CMethodParameter::CMethodParameter(std::string name, DataType* dataType, bool pointer) :
    name(name),
    dataType(dataType),
    pointer(pointer)
{

}

bool CMethodParameter::sameSignature(CMethodParameter &compare) {
    return ((dataType == compare.dataType) && (pointer == compare.pointer));
}

CMethod::CMethod(std::string name, CMethodParameter* returnType) :
    name(name),
    returnType(returnType)
{
}

CMethod::~CMethod() {
    delete returnType;
    for (list<CMethodParameter*>::iterator i = parameters.begin(); i != parameters.end(); i++) {
        delete *i;
    }
}

#include <iostream>
#include "datatype.h"
bool CMethod::sameSignature(CMethod &compare) {
    if (!(returnType->sameSignature(*compare.returnType)))
        return false;
    if (parameters.size() != compare.parameters.size())
        return 0;
    list<CMethodParameter*>::iterator comp_i = compare.parameters.begin();
    for (list<CMethodParameter*>::iterator i = parameters.begin(); i != parameters.end(); i++) {
        if (!((*i)->sameSignature((**comp_i))))
            return false;
        comp_i++;
    }

    return true;
}

