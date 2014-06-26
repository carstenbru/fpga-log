#include "cparameter.h"

CParameter::CParameter(std::string name, DataType* dataType, bool pointer) :
    name(name),
    dataType(dataType),
    pointer(pointer)
{
}

CParameter::CParameter(std::string name, DataType* dataType, bool pointer, std::string value) :
    name(name),
    dataType(dataType),
    pointer(pointer),
    value(value)
{
}

bool CParameter::sameSignature(CParameter &compare) {
    return ((dataType == compare.dataType) && (pointer == compare.pointer));
}
