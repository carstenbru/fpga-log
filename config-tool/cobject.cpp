#include "cobject.h"

CObject::CObject(std::string name, DataTypeStruct* dataType) :
    name(name),
    type(dataType)
{
}
