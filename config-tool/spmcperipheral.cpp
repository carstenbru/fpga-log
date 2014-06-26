#include "spmcperipheral.h"
#include <QXmlStreamReader>
#include <QFile>
#include <iostream>
#include <QProcessEnvironment>

using namespace std;

SpmcPeripheral::SpmcPeripheral(DataType *type) :
    dataType(type)
{
    readParametersFromFile();
}

SpmcPeripheral::~SpmcPeripheral() {
    for (list<CParameter*>::iterator i = parameters.begin(); i != parameters.end(); i++) {
        delete *i;
    }
}

string SpmcPeripheral::getFileName() {
    string spmcRoot = QProcessEnvironment::systemEnvironment().value("SPARTANMC_ROOT").toStdString();
    string periName = dataType->getName();
    periName.erase(periName.length() - 7, periName.length());
    return spmcRoot + "/spartanmc/hardware/" + periName + "/module.xml";
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
        string type = attributes.value("type").toString().toStdString(); //TODO with choice attributes as enumeration
        if (type.compare("int") == 0)
            type = "peripheral_int";
        while (reader.readNextStartElement()) {
            if (reader.name().toString().compare("range") == 0) {
                QXmlStreamAttributes attributes = reader.attributes();
                int min = attributes.value("min").toString().toInt();
                int max = attributes.value("max").toString().toInt();
                type = dataType->getName() + "_" + paramName;
                try {
                    DataType::getType(type);
                } catch (exception) {
                    new DataTypeNumber(type, min, max);
                }
            }
            reader.skipCurrentElement();
        }
        string value = attributes.value("value").toString().toStdString();
        parameters.push_back(new CParameter(paramName, DataType::getType(type), false, value));
    }
}
