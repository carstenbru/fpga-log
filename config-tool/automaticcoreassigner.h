#ifndef AUTOMATICCOREASSIGNER_H
#define AUTOMATICCOREASSIGNER_H

#include <QXmlStreamWriter>
#include <QFile>
#include <map>
#include "datalogger.h"
#include "datatype.h"
#include "outputgenerator.h"

class AutomaticCoreAssigner : public QObject
{
Q_OBJECT
public:
    AutomaticCoreAssigner();

    void calculateModuleWeights();
    void readModuleWeights();
private:
    void saveLogger(DataLogger* dataLogger);
    void addObject(DataLogger* logger, DataTypeStruct* dataType);
    void measureNextModule();
private slots:
    void firmwareSizeResult(int size);

private:
    QXmlStreamWriter* weightWriter;
    QFile* weightFile;
    std::string lastMeasuredModule;
    OutputGenerator* outputGenerator;
    std::map<std::string, DataTypeStruct*> types;
    std::map<std::string, DataTypeStruct*>::iterator modulesPending;
    std::map<std::string, DataTypeStruct*>::iterator modulesPendingEnd;
    int baseSize;
};

#endif // AUTOMATICCOREASSIGNER_H
