/**
 * @file automaticcoreassigner.h
 * @brief AutomaticCoreAssigner class
 *
 * @author Carsten Bruns (carst.bruns@gmx.de)
 */

#ifndef AUTOMATICCOREASSIGNER_H
#define AUTOMATICCOREASSIGNER_H

#include <QXmlStreamWriter>
#include <QFile>
#include <map>
#include <unordered_set>
#include "datatype.h"
#include "outputgenerator.h"
#include "datastreamobject.h"

//default maximum weight that should be assigned to a single core
#define DEFAULT_MAX_WEIGHT_PER_CORE 40000 //TODO adjust
//default maximum number of SpartanMC cores in a single system
#define DEFAULT_MAX_CORES 128
//default weight of a data stream connector
#define DEFAULT_DATA_STREAM_WEIGHT 3
//default weight of a control stream connector
#define DEFAULT_CONTROL_STREAM_WEIGHT 2

//DataLogger forward declariation,including the header here would lead to a cycle
class DataLogger;

/**
 * @brief abstract class for weight heuristic for core assignment
 */
class WeightHeuristic {
public:
    /**
     * @brief calculates the total score from the previously calculated values
     *
     * @param weightDistribution weight distribution factor between the different cores
     * @param connectorWeight sum of the connector weights for all used core connectors
     * @return calculated weight value
     */
    virtual float calculateTotalScore(float weightDistribution, int connectorWeight) = 0;
};

/**
 * @brief default implementation of weight heuristic for core assignment
 */
class WeightHeuristicDefault : public WeightHeuristic {
public:
    virtual float calculateTotalScore(float weightDistribution, int connectorWeight) {
        return 1.0f/weightDistribution * (connectorWeight+1);
    }
};

/**
 * @brief class responsible for automatically assigning objects to different cores
 *
 * This class can assign objects automatically to different cores.
 * Also it is used to calulate weights for every module type in the first place.
 */
class AutomaticCoreAssigner : public QObject
{
Q_OBJECT
public:
    /**
     * @brief constructor
     */
    AutomaticCoreAssigner();

    /**
     * @brief calculates weights for every module and writes them to a file
     *
     * The weights are calculated by creating a logger with only one object and compiling the code. From the resulting size the size of a logger
     * with no objects is subtracted to get a weight value.
     * The results are then written to a file.
     * This only has to be done when a new module was added to fpga-log or an existing one was changed.
     */
    void calculateModuleWeights();

    /**
     * @brief reads in the module weights from the weights file
     */
    void readModuleWeights();

    /**
     * @brief sets the minimum number of SpartanMC cores tried for the assignment
     *
     * @param minCores new minimum number of cores
     */
    void setMinCores(int minCores) { this->minCores = minCores; }

    /**
     * @brief setMaxCores sets the maximum number of SpartanMC cores tried for the assignment
     *
     * @param maxCores new maximum number of cores
     */
    void setMaxCores(int maxCores) { this->maxCores = maxCores; }

    /**
     * @brief sets the maximum weight that should be assigned to a single core
     *
     * @param maxWeightPerCore new maximum weight that should be assigned to a single core
     */
    void setMaxWeightPerCore(int maxWeightPerCore) { this->maxWeightPerCore = maxWeightPerCore; }

    /**
     * @brief sets the weight of a data stream connector for the heuristic
     *
     * @param dataStreamWeight new weight of a data stream connector
     */
    void setDataStreamWeight(int dataStreamWeight) { this->dataStreamWeight = dataStreamWeight; }

    /**
     * @brief sets the weight of a control stream connector for the heuristic
     *
     * @param controlStreamWeight new weight of a control stream connector
     */
    void setControlStreamWeight(int controlStreamWeight) { this->controlStreamWeight = controlStreamWeight; }

    /**
     * @brief gets the minimum number of SpartanMC cores tried for the assignment
     *
     * @return minimum number of cores
     */
    int getMinCores() { return minCores; }

    /**
     * @brief gets the maximum number of SpartanMC cores tried for the assignment
     *
     * @return maximum number of cores
     */
    int getMaxCores() { return maxCores; }

    /**
     * @brief gets the maximum weight that should be assigned to a single core
     *
     * @return the maximum weight that should be assigned to a single core
     */
    int getMaxWeightPerCore() { return maxWeightPerCore; }

    /**
     * @brief gets the weight of a data stream connector for the heuristic
     *
     * @return the weight of a data stream connector for the heuristic
     */
    int getDataStreamWeight() { return dataStreamWeight; }

    /**
     * @brief gets the weight of a control stream connector for the heuristic
     *
     * @return the weight of a control stream connector for the heuristic
     */
    int getControlStreamWeight() { return controlStreamWeight; }

    /**
     * @brief starts the automatic core assignment for a datalogger
     *
     * @param dataLogger destination datalogger
     * @return true if the assignment was successful
     */
    bool assignCores(DataLogger* dataLogger);

    /**
     * @brief serialization operator, writes the core assigner to a XML stream
     * @param out XML stream destination
     * @param aco core assigner to write
     * @return serialization operator, writes the object to a XML stream
     */
    friend QXmlStreamWriter& operator<<(QXmlStreamWriter& out, AutomaticCoreAssigner& aco);

    /**
     * @brief serialization operator, reads the core assigner from a XML stream
     * @param in XML stream source
     * @param aco core assigner destination
     * @return source XML stream for operator concatenation
     */
    friend QXmlStreamReader& operator>>(QXmlStreamReader& in, AutomaticCoreAssigner& aco);
private:
    /**
     * @brief saves a created test datalogger as test system
     *
     * @see calculateModuleWeights
     * @param dataLogger the datalogger to save
     */
    void saveLogger(DataLogger* dataLogger);
    /**
     * @brief adds an object to the logger and sets all parameters to "0"
     *
     * @param logger the logger to change
     * @param dataType type of the new object
     */
    void addObject(DataLogger* logger, DataTypeStruct* dataType);
    /**
     * @brief starts size measurement of the next module
     */
    void measureNextModule();

    /**
     * @brief starts the automatic core assignment for a datalogger
     *
     * @param dataLogger datalogger to assign the objects to cores
     * @param weights weights of the used modules, including sub-modules/used objects
     * @param totalWeight total system weight
     * @param connections map of module connections (key1: source, key2: destination, value: connections weight between the objects in the keys)
     * @param cores number of cores to try the assignment for
     * @return true if the assignment was successful
     */
    bool assignCores(DataLogger* dataLogger, std::map<CObject *, int> weights, int totalWeight, std::map<DatastreamObject *, std::map<DatastreamObject *, int> > &connections, int cores);

    /**
     * @brief calculates the real weights of all DatastreamModules in the DataLogger, including their helper modules
     *
     * e.g. formatter and control_protocol for a sink_uart_t) which have to be on the same core
     *
     * @param dataLogger datalogger to work on
     * @param totalWeight total system weight to add up (will be changed!)
     * @return map of weights to all objects in the system
     */
    std::map<CObject*, int> getRealDatastreamModuleWeights(DataLogger* dataLogger, int &totalWeight);
    /**
     * @brief calculates the real weight of one object, including its helper objects which have to be on the same core
     *
     * @param object object to calculate the weight of
     * @param objects map of all objects in the system
     * @return weight value of the pbject
     */
    int getRealObjectWeight(CObject* object, std::map<std::string, CObject*> objects);

    /**
     * @brief analyzes the connections in the system and assignes weight values to all module pairings (i.e. the cost of seperating them)
     *
     * @param dataLogger datalogger to work on
     * @param connections map of module connections (key1: source, key2: destination, value: connections weight between the objects in the keys)
     */
    void analyzeConnections(DataLogger* dataLogger, std::map<DatastreamObject *, std::map<DatastreamObject *, int> > &connections);
    /**
     * @brief generates a partioning of the system into sets of objects
     *
     * @param dataLogger datalogger to work on
     * @param cores number of cores in the system
     * @param weights map of weights to all objects in the system, see getRealDatastreamModuleWeights
     * @param weightPerCore approximate weight that should be on one core after partitioning
     * @param connections connections map of module connections (key1: source, key2: destination, value: connections weight between the objects in the keys)
     * @param shuffle shuffle flag as seed to start the algorithm with different start values to find the best result
     * @param totalWeight total weigth of the system
     * @param score quality score of the found solution (return value)
     * @return partioning of the system into sets of objects (key: set, value: list of objects in this set)
     */
    std::map<int, std::list<DatastreamObject*>> generatePartition(DataLogger* dataLogger, int cores, std::map<CObject*, int> weights, int weightPerCore, std::map<DatastreamObject*, std::map<DatastreamObject*, int>>& connections, int shuffle, int totalWeight, float &score);
    /**
     * @brief finds the object that fits best to a set and therefore should be added to it
     *
     * @param currentSet current set of objects
     * @param done set of objects which are already assigned
     * @param queue list of objects to assign
     * @param connections connections map of module connections (key1: source, key2: destination, value: connections weight between the objects in the keys)
     * @return the object that fits best to a set and therefore should be added to it
     */
    DatastreamObject* findNextObject(std::list<DatastreamObject*> currentSet, std::unordered_set<DatastreamObject*>& done, std::list<DatastreamObject*>& queue, std::map<DatastreamObject*, std::map<DatastreamObject*, int>>& connections);
    /**
     * @brief evaluates a partition, i.e. calculates a score value for it
     *
     * @param sets partitioning into sets of objects
     * @param connections connections map of module connections (key1: source, key2: destination, value: connections weight between the objects in the keys)
     * @param setWeights accumulated weight values of each set (key: set, value: weight)
     * @param totalWeight total weight of the system
     * @param cores number of cores in the assignment
     * @return score for the partition
     */
    float evaluatePartition(std::map<int, std::list<DatastreamObject*>>& sets, std::map<DatastreamObject*, std::map<DatastreamObject*, int>>& connections, std::map<int, int>& setWeights, int totalWeight, int cores);
private slots:
    /**
     * @brief slot for receiving the result of a firmware size measurement
     *
     * @param size measured firmware size
     */
    void firmwareSizeResult(int size);

private:
    QXmlStreamWriter* weightWriter; /**< weight file XML writer */
    QFile* weightFile; /**< weight file */
    std::string lastMeasuredModule; /**< name of the last measured module (to write in result file) */
    OutputGenerator* outputGenerator; /**< used output generator (for module size measurement) */
    std::map<std::string, DataTypeStruct*> types; /**< map of all known data types (for module size measurement) */
    std::map<std::string, DataTypeStruct*>::iterator modulesPending; /**< iterator to next type to measure (for size measurement) */
    std::map<std::string, DataTypeStruct*>::iterator modulesPendingEnd; /**< iterator pointing at the last type to measure (size measurement) */
    int baseSize; /**< base firmware size without any modules */

    int maxCores; /**< maximum number of cores to use */
    int minCores; /**< minimum number of cores to use */
    int maxWeightPerCore; /**< maximum weight for one core */
    int dataStreamWeight; /**< weight of a data stream conector */
    int controlStreamWeight; /**< weight of a control stream conector */

    WeightHeuristic* heuristic; /**< instance of the used heuristic object */

    static int averageWeight; /**< average weight of all modules (to use for unknown objects) */
signals:
    /**
     * @brief emitted when a warning was found during core assignment
     *
     * @param message warning message describing the problem
     */
    void warningFound(std::string message);
};

#endif // AUTOMATICCOREASSIGNER_H
