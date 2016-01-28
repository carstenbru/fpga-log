#include "cobject.h"
#include "datalogger.h"
#include <QFile>
#include <iostream>

using namespace std;

CObject::CObject(std::string name, DataTypeStruct* dataType, DataLogger* dataLogger) :
    name(name),
    type(dataType),
    initMethod(NULL),
    definitionsUpdated(false)
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

CObject::CObject(QXmlStreamReader& in, DataLogger* dataLogger, string name, bool ignorePins) :
    CObject(in, dataLogger, false, name, ignorePins)
{

}

bool CObject::setPeripheral(SpmcPeripheral* newPeripheral) {
    for (list<SpmcPeripheral*>::iterator i = peripherals.begin(); i != peripherals.end(); i++) {
        if ((*i)->getCompleteName().compare(newPeripheral->getCompleteName()) == 0) {
            delete *i;
            *i = newPeripheral;
            return true;
        }
    }
    return false;
}

//the dummy bool parameter is needed by the DatastreamObject class,
//so that it can move the XML reader in the initialization list
CObject::CObject(QXmlStreamReader& in, DataLogger* dataLogger, bool, string name, bool ignorePins) :
    CObject(in.attributes().value("name").toString().toStdString(),
            DataTypeStruct::getType(in.attributes().value("type").toString().toStdString()),
            dataLogger)
{
    if (!name.empty()) {
        this->name = name;
    }

    while (in.readNextStartElement()) {
        if (in.name().compare("SpmcPeripheral") == 0) {
            if (!setPeripheral(new SpmcPeripheral(in, this, dataLogger, ignorePins))) {
                definitionsUpdated = true;
            }
        } else if (in.name().compare("CMethod") == 0) {
            if (in.attributes().value("name").compare("init") == 0) {
               if (initMethod != NULL)
                    delete initMethod;
                initMethod = new CMethod(in, type->getMethod("init"), this);
            } else {
                CMethod* methodSig = type->getMethod(in.attributes().value("name").toString().toStdString());
                if (methodSig != NULL) {
                    advancedConfig.push_back(new CMethod(in, methodSig, this));
                } else {
                    definitionsUpdated = true;
                }
            }
        } else if (in.name().compare("timestamp_pin") == 0) {
            if (!ignorePins) {
                string parameter = in.attributes().value("name").toString().toStdString();
                try {
                    timestampPinsInvert.at(parameter) = (in.attributes().value("inverted").toString().compare("true") == 0);
                    in.readNextStartElement();
                    CParameter* param = new CParameter(in);
                    if (timestampPins.at(parameter)->getName().compare(param->getName()) != 0) {
                        param->setName(timestampPins.at(parameter)->getName());
                        definitionsUpdated = true;
                    }
                    timestampPins[parameter] = param;
                } catch (exception e) {
                    definitionsUpdated = true;
                }
            }

            in.skipCurrentElement();
        } else
            in.skipCurrentElement();
    }
    connect(this, SIGNAL(advancedConfigRemoved()), dataLogger, SLOT(parameterChanged()));

    if (definitionsUpdated) {
        dataLogger->addDefinitionsUpdatedModule(this->name);
    }
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
