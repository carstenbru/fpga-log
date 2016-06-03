#ifndef AUTOMATICCOREASSIGNER_H
#define AUTOMATICCOREASSIGNER_H

#include <QXmlStreamWriter>
#include <QFile>
#include <map>
#include <unordered_set>
#include "datatype.h"
#include "outputgenerator.h"
#include "datastreamobject.h"

#define DEFAULT_MAX_WEIGHT_PER_CORE 40000 //TODO adjust
#define DEFAULT_MAX_CORES 128
#define DEFAULT_DATA_STREAM_WEIGHT 3
#define DEFAULT_CONTROL_STREAM_WEIGHT 2

class DataLogger;

class AutomaticCoreAssigner : public QObject
{
Q_OBJECT
public:
    AutomaticCoreAssigner();

    void calculateModuleWeights();
    void readModuleWeights();

    void setMaxCores(int maxCores) { this->maxCores = maxCores; }
    void setMaxWeightPerCore(int maxWeightPerCore) { this->maxWeightPerCore = maxWeightPerCore; }
    bool assignCores(DataLogger* dataLogger);
    bool assignCores(DataLogger* dataLogger, std::map<CObject *, int> weights, int totalWeight, std::map<DatastreamObject *, std::map<DatastreamObject *, int> > &connections, int cores);

    friend QXmlStreamWriter& operator<<(QXmlStreamWriter& out, AutomaticCoreAssigner& aco);
    friend QXmlStreamReader& operator>>(QXmlStreamReader& in, AutomaticCoreAssigner& aco);
private:
    void saveLogger(DataLogger* dataLogger);
    void addObject(DataLogger* logger, DataTypeStruct* dataType);
    void measureNextModule();

    /*
     * calculates the real weights of all DatastreamModules in the DataLogger, including their helper modules
     * (e.g. formatter and control_protocol for a sink_uart_t) which have to be on the same core
     */
    std::map<CObject*, int> getRealDatastreamModuleWeights(DataLogger* dataLogger, int &totalWeight);
    /*
     * calculates the real weight of one object, including its helper objects which have to be on the same core
     */
    int getRealObjectWeight(CObject* object, std::map<std::string, CObject*> objects);

    void analyzeConnections(DataLogger* dataLogger, std::map<DatastreamObject *, std::map<DatastreamObject *, int> > &connections);
    std::map<int, std::list<DatastreamObject*>> generatePartition(DataLogger* dataLogger, int cores, std::map<CObject*, int> weights, int weightPerCore, std::map<DatastreamObject*, std::map<DatastreamObject*, int>>& connections, int shuffle, int totalWeight, float &score);
    DatastreamObject* findNextObject(std::list<DatastreamObject*> currentSet, std::unordered_set<DatastreamObject*>& done, std::list<DatastreamObject*>& queue, std::map<DatastreamObject*, std::map<DatastreamObject*, int>>& connections);
    float evaluatePartition(std::map<int, std::list<DatastreamObject*>>& sets, std::map<DatastreamObject*, std::map<DatastreamObject*, int>>& connections, std::map<int, int>& setWeights, int totalWeight, int cores);
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

    int maxCores;
    int minCores;
    int maxWeightPerCore;
    int dataStreamWeight;
    int controlStreamWeight;

    static int averageWeight;
signals:
    void warningFound(std::string message);
};

#endif // AUTOMATICCOREASSIGNER_H
