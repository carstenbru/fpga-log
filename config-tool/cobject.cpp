#include "cobject.h"
#include "datalogger.h"
#include <QFile>

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

        readTimestampPinsFromModuleXml();
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
        } else if (in.name().compare("timestamp_pin") == 0) {
            string parameter = in.attributes().value("name").toString().toStdString();
            timestampPinsInvert[parameter] = (in.attributes().value("inverted").toString().compare("true") == 0);
            in.readNextStartElement();
            timestampPins[parameter] = new CParameter(in);
            in.skipCurrentElement();
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
    for (map<std::string, CParameter*>::iterator i = timestampPins.begin(); i != timestampPins.end(); i++) {
        delete i->second;
    }
}

void CObject::readTimestampPinsFromModuleXml() {
    string moduleXmlFile = "../config-tool-files/modules/";
    moduleXmlFile += type->getCleanedName();
    moduleXmlFile += ".xml";
    QFile file(QString(moduleXmlFile.c_str()));
    if (!file.open(QIODevice::ReadOnly)) {
        return;
    }
    QXmlStreamReader reader(&file);
    reader.readNextStartElement();
    while (reader.readNextStartElement()) {
        if (reader.name().compare("timestamp_pin") == 0) {
            string refParam = reader.attributes().value("parameter").toString().toStdString();
            timestampPins[refParam] =
                    new CParameter(reader.attributes().value("name").toString().toStdString(), DataTypePin::getPinType());
            initMethod->getParameter(refParam)->setHideFromUser(true);

            QStringRef invStr = reader.attributes().value("invert");
            bool invert = false;
            if (!invStr.isEmpty()) {
                invert = (invStr.toString().compare("TRUE") == 0);
            }
            timestampPinsInvert[refParam] = invert;
        }
        reader.skipCurrentElement();
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

    for (map<string, CParameter*>::iterator i = cObject.timestampPins.begin(); i != cObject.timestampPins.end(); i++) {
        out.writeStartElement("timestamp_pin");
        out.writeAttribute("name", i->first.c_str());
        out.writeAttribute("inverted", cObject.timestampPinsInvert[i->first] ? "true" : "false");

        out << *(i->second);

        out.writeEndElement();
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

list<CParameter*> CObject::getTimestampPinParameters() {
    list<CParameter*> res;
    for (map<string, CParameter*>::iterator i = timestampPins.begin(); i != timestampPins.end(); i++) {
        res.push_back(i->second);
    }
    return res;
}
