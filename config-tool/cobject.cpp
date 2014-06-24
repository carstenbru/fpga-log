#include "cobject.h"

CObject::CObject(std::string name, DataType* dataType) :
    name(name),
    type(dataType)
{
}
