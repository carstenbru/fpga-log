#include "spmcperipheral.h"
#include <QXmlStreamReader>
#include <QFile>
#include <iostream>
#include <QProcessEnvironment>
#include <QDirIterator>
#include "datalogger.h"

using namespace std;

std::map<std::string, std::string> SpmcPeripheral::peripheralXMLs;

SpmcPeripheral::SpmcPeripheral(DataType *type, string parentModuleName, DataLogger* dataLogger) :
    dataLogger(dataLogger),
    dataType(type),
    parentModuleName(parentModuleName)
{
    readParametersFromFile();
    readModuleXML();
}

SpmcPeripheral::~SpmcPeripheral() {
    for (list<CParameter*>::iterator i = parameters.begin(); i != parameters.end(); i++) {
        delete *i;
    }
}

CParameter* SpmcPeripheral::getParameter(std::string name) {
    for (std::list<CParameter*>::iterator i = parameters.begin(); i != parameters.end(); i++) {
        if ((*i)->getName().compare(name) == 0)
            return *i;
    }
    return NULL;
}

std::string SpmcPeripheral::readModuleNameFromFile(std::string fileName) {
    QFile file(QString(fileName.c_str()));
    if (!file.open(QIODevice::ReadOnly)) {
        cerr << "unable to open peripheral module xml file: " << fileName << endl;
        return "unable to open";
    }
    QXmlStreamReader reader(&file);
    reader.readNextStartElement();
    return reader.attributes().value("name").toString().toStdString();
}

void SpmcPeripheral::loadPeripheralXMLs() {
    string path = QProcessEnvironment::systemEnvironment().value("SPARTANMC_ROOT").toStdString();
    path += "/spartanmc/hardware/";
    QDirIterator dirIter(QString(path.c_str()), (QDirIterator::Subdirectories | QDirIterator::FollowSymlinks));
    while (dirIter.hasNext()) {
        dirIter.next();
        if (QFileInfo(dirIter.filePath()).isFile()) {
            if (dirIter.fileName() == "module.xml") {
                string fileName = dirIter.filePath().toStdString();
                peripheralXMLs[readModuleNameFromFile(fileName)] = fileName;
            }
        }
    }
}

string SpmcPeripheral::getFileName() {
    string periName = dataType->getName();
    periName.erase(periName.length() - 7, periName.length());
    try {
        return peripheralXMLs.at(periName);
    } catch (exception) {
        cerr << "unable to find peripheral module xml file: " << dataType->getName() << endl;
        return "not found";
    }
}

void SpmcPeripheral::readParametersFromFile() {
    QFile file(QString(getFileName().c_str()));
    if (!file.open(QIODevice::ReadOnly)) {
        cerr << "unable to open peripheral module xml file: " << dataType->getName() << endl;
        return;
    }
    QXmlStreamReader reader(&file);
    while (reader.name().compare("parameters") != 0) {
        reader.readNextStartElement();
    }
    while (reader.readNextStartElement()) {
        QXmlStreamAttributes attributes = reader.attributes();
        string paramName = attributes.value("name").toString().toStdString();
        string type = attributes.value("type").toString().toStdString();
        if (type.compare("int") == 0)
            type = "peripheral_int";
        DataTypeEnumeration* dt = NULL;
        while (reader.readNextStartElement()) {
            type = dataType->getName() + "_" + paramName;
            if (reader.name().toString().compare("range") == 0) {
                try {
                    DataType::getType(type);
                } catch (exception) {
                    QXmlStreamAttributes attributes = reader.attributes();
                    int min = attributes.value("min").toString().toInt();
                    int max = attributes.value("max").toString().toInt();
                    new DataTypeNumber(type, min, max);
                }
            } else if (reader.name().toString().compare("choice") == 0) {
                if (dt == NULL) {
                    try {
                        DataType::getType(type);
                    } catch (exception) {
                        dt = new DataTypeEnumeration(type);
                        dt->addValue(reader.attributes().value("value").toString().toStdString());
                    }
                } else
                    dt->addValue(reader.attributes().value("value").toString().toStdString());
            }
            reader.skipCurrentElement();
        }
        string value = attributes.value("value").toString().toStdString();
        CParameter* param = new CParameter(paramName, DataType::getType(type), false, value);
        parameters.push_back(param);
        if (paramName.compare("CLOCK_FREQUENCY") == 0)
            param->setHideFromUser(true);
    }
}

void SpmcPeripheral::readModuleXML() {
    string moduleXmlFile = "../modules/";
    moduleXmlFile += parentModuleName;
    moduleXmlFile += ".xml";
    QFile file(QString(moduleXmlFile.c_str()));
    if (!file.open(QIODevice::ReadOnly)) {
        return;
    }
    QXmlStreamReader reader(&file);
    reader.readNextStartElement();
    while (reader.readNextStartElement()) {
        if (reader.name().toString().compare("parameter") == 0) {
            QXmlStreamAttributes attributes = reader.attributes();
            CParameter* parameter = getParameter(attributes.value("name").toString().toStdString());
            QStringRef value = attributes.value("value");
            if (!value.isEmpty()) {
                parameter->setValue(value.toString().toStdString());
            }
            QStringRef hideStr = attributes.value("hide");
            if (!hideStr.isEmpty()) {
                bool hide = (hideStr.toString().compare("TRUE") == 0);
                parameter->setHideFromUser(hide);
            }
        }
        reader.skipCurrentElement();
    }
}
