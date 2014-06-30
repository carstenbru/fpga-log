#include "cparameter.h"
#include "datatype.h"

CParameter::CParameter(std::string name, DataType* dataType, bool pointer) :
    QObject(),
    name(name),
    value(dataType->getDefaultValue()),
    hideFromUser(false),
    critical(false),
    dataType(dataType),
    pointer(pointer)
{
}

CParameter::CParameter(std::string name, DataType* dataType, bool pointer, std::string value) :
    name(name),
    value(value),
    hideFromUser(false),
    critical(false),
    dataType(dataType),
    pointer(pointer)
{
}

CParameter::CParameter(const CParameter& other) :
    QObject(),
    name(other.name),
    value(other.value),
    hideFromUser(other.hideFromUser),
    critical(false),
    dataType(other.dataType),
    pointer(other.pointer)
{

}

bool CParameter::sameSignature(CParameter &compare) {
    return ((dataType == compare.dataType) && (pointer == compare.pointer));
}
