#include "cparameter.h"

CParameter::CParameter(std::string name, DataType* dataType, bool pointer) :
    name(name),
    hideFromUser(false),
    dataType(dataType),
    pointer(pointer)
{
}

CParameter::CParameter(std::string name, DataType* dataType, bool pointer, std::string value) :
    name(name),
    value(value),
    hideFromUser(false),
    dataType(dataType),
    pointer(pointer)
{
}

bool CParameter::sameSignature(CParameter &compare) {
    return ((dataType == compare.dataType) && (pointer == compare.pointer));
}
