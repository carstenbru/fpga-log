#include "cobject.h"
#include "datalogger.h"

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
                peripherals.push_back(new SpmcPeripheral((*i).getName(), (*i).getDataType(), this, dataLogger));
            }
        }
        connect(this, SIGNAL(advancedConfigRemoved()), dataLogger, SLOT(parameterChanged()));
    }
}

CObject::CObject(QXmlStreamReader& in, DataLogger* dataLogger) :
    CObject(in, dataLogger, false)
{

}

CObject::CObject(QXmlStreamReader& in, DataLogger* dataLogger, bool readStart) {
    if (readStart) {
        in.readNextStartElement();
    }

    const QXmlStreamAttributes& a = in.attributes();

    name = a.value("name").toString().toStdString();
    type = DataTypeStruct::getType(a.value("type").toString().toStdString());

    while (in.readNextStartElement()) {
        if (in.name().compare("SpmcPeripheral") == 0) {
            peripherals.push_back(new SpmcPeripheral(in, this, dataLogger));
        } else if (in.name().compare("CMethod") == 0) {
            if (in.attributes().value("name").compare("init") == 0)
                initMethod = new CMethod(in);
            else
                advancedConfig.push_back(new CMethod(in));
        } else
            in.skipCurrentElement();
    }
    connect(this, SIGNAL(advancedConfigRemoved()), dataLogger, SLOT(parameterChanged()));
}

CObject::~CObject() {
    delete initMethod;
    for (list<SpmcPeripheral*>::iterator i = peripherals.begin(); i != peripherals.end(); i++) {
        delete *i;
    }
}

QXmlStreamWriter& operator<<(QXmlStreamWriter& out, CObject& cObject) {
    out.writeStartElement("CObject");

    out.writeAttribute("name", cObject.name.c_str());
    out.writeAttribute("type", cObject.type->getName().c_str());
    out << *cObject.initMethod;

    for (list<SpmcPeripheral*>::iterator i = cObject.peripherals.begin(); i != cObject.peripherals.end(); i++) {
        out << **i;
    }

    for (list<CMethod*>::iterator i = cObject.advancedConfig.begin(); i != cObject.advancedConfig.end(); i++) {
        out << **i;
    }

    out.writeEndElement();
    return out;
}

void CObject::addAdvancedConfig(std::string methodName) {
    advancedConfig.push_back(new CMethod(*type->getMethod(methodName)));
}

void CObject::removeAdvancedConfig(int methodId) {
    list<CMethod*>::iterator i = advancedConfig.begin();
    advance(i, methodId);
    CMethod* method = *i;
    advancedConfig.erase(i);
    emit advancedConfigRemoved();
    delete method;
}
