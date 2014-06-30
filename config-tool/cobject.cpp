#include "cobject.h"

using namespace std;

CObject::CObject(std::string name, DataTypeStruct* dataType, DataLogger* dataLogger) :
    name(name),
    type(dataType),
    initMethod(NULL)
{
    if (type->isInstantiableObject()) {
        initMethod = new CMethod(*type->getMethod("init"));

        list<CParameter>* parameters = initMethod->getParameters();
        list<CParameter>::iterator i = parameters->begin();
        i++;
        for (; i != parameters->end(); i++) {
            if ((*i).getDataType()->hasSuffix("_regs_t")) {
                peripherals.push_back(new SpmcPeripheral((*i).getName(), (*i).getDataType(), type->getCleanedName(), dataLogger));
            }
        }
    }
}

CObject::~CObject() {
    delete initMethod;
    for (list<SpmcPeripheral*>::iterator i = peripherals.begin(); i != peripherals.end(); i++) {
        delete *i;
    }
}
