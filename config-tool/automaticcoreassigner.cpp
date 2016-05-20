#include "automaticcoreassigner.h"
#include <iostream>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QApplication>
#include "outputgenerator.h"

using namespace std;

AutomaticCoreAssigner::AutomaticCoreAssigner() {

}

void AutomaticCoreAssigner::saveLogger(DataLogger* dataLogger) {
    QFile file("./tmp/measure.xml");
    QDir().mkpath(QFileInfo("./tmp/measure.xml").dir().absolutePath());
    file.open(QIODevice::WriteOnly);

    if (file.isOpen()) {
        QXmlStreamWriter writer(&file);
        writer.setAutoFormatting(true);

        writer.writeStartDocument();
        writer << *dataLogger;
        writer.writeEndDocument();

        file.close();
    } else {
        cerr << "Fehler: Zieldatei konnte nicht geschrieben werden." << endl;
    }
}

void AutomaticCoreAssigner::addObject(DataLogger* logger, DataTypeStruct* dataType) {
    string name = logger->newObject(dataType);
    CObject* object = logger->getObjectsMap()[name];

    list<CParameter>* parameters = object->getInitMethod()->getParameters();
    list<CParameter>::iterator i = parameters->begin();
    i++;
    for (; i != parameters->end(); i++) {
        DataType* paramType = (*i).getDataType();
        if (!paramType->hasSuffix("_regs_t") && !paramType->hasSuffix("_dma_t")) {
            if (!(*i).getHideFromUser()) {
               (*i).setValue("0");
            }
        }
    }
}

void AutomaticCoreAssigner::measureNextModule() {
    if (modulesPending != modulesPendingEnd) {
        DataTypeStruct* type = modulesPending->second;
        modulesPending++;
        if (type->isInstantiableObject()) {
            DataLogger logger;
            lastMeasuredModule = type->getName();
            addObject(&logger, type);
            saveLogger(&logger);
            delete outputGenerator;
            outputGenerator = new OutputGenerator(&logger, "./tmp", false);
            connect(outputGenerator, SIGNAL(firmwareSizeResult(int)), this, SLOT(firmwareSizeResult(int)));
            outputGenerator->measureFirmwareSize();
        } else {
            measureNextModule();
        }
    } else {
        weightWriter->writeEndElement();
        weightWriter->writeEndDocument();
        weightFile->close();
        delete weightFile;
        delete weightWriter;
    }
}

void AutomaticCoreAssigner::firmwareSizeResult(int size) {
    size -= baseSize;

    cout << lastMeasuredModule << " has weight "  << to_string(size) << endl;
    weightWriter->writeStartElement("module");
    weightWriter->writeAttribute("name", lastMeasuredModule.c_str());
    weightWriter->writeAttribute("weight", to_string(size).c_str());
    weightWriter->writeEndElement();

    if (lastMeasuredModule.compare("base") == 0) {
        baseSize = size;
    }

    measureNextModule();
}

void AutomaticCoreAssigner::calculateModuleWeights() {
    cout << "calculating module weights!" << endl;
    outputGenerator = NULL;
    baseSize = 0;

    weightFile = new QFile("../config-tool-files/module-weights.xml");
    QDir().mkpath(QFileInfo("../config-tool-files/module-weights.xml").dir().absolutePath());
    weightFile->open(QIODevice::WriteOnly);
    weightWriter = new QXmlStreamWriter(weightFile);
    weightWriter->setAutoFormatting(true);

    weightWriter->writeStartDocument();
    weightWriter->writeStartElement("module-weights");

    types = DataTypeStruct::getTypes();
    modulesPending = types.begin();
    modulesPendingEnd = types.end();

    DataLogger logger;
    lastMeasuredModule = "base";
    saveLogger(&logger);
    outputGenerator = new OutputGenerator(&logger, "./tmp", false);
    connect(outputGenerator, SIGNAL(firmwareSizeResult(int)), this, SLOT(firmwareSizeResult(int)));
    outputGenerator->measureFirmwareSize();
}
