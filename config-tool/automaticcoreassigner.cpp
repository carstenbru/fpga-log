#include "automaticcoreassigner.h"
#include <iostream>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QApplication>
#include <math.h>
#include "outputgenerator.h"
#include "datalogger.h"

using namespace std;

int AutomaticCoreAssigner::averageWeight;

AutomaticCoreAssigner::AutomaticCoreAssigner() :
    maxCores(DEFAULT_MAX_CORES),
    minCores(1),
    maxWeightPerCore(DEFAULT_MAX_WEIGHT_PER_CORE),
    dataStreamWeight(DEFAULT_DATA_STREAM_WEIGHT),
    controlStreamWeight(DEFAULT_CONTROL_STREAM_WEIGHT),
    heuristic(new WeightHeuristicDefault())
{

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

void AutomaticCoreAssigner::readModuleWeights() {
    QFile file("../config-tool-files/module-weights.xml");
    file.open(QIODevice::ReadOnly);

    averageWeight = 0;
    int valCount = 0;

    if (file.isOpen()) {
        QXmlStreamReader reader(&file);
        reader.readNextStartElement();

        while (reader.readNextStartElement()) {
            const QXmlStreamAttributes& a = reader.attributes();
            string name = a.value("name").toString().toStdString();
            int weight = a.value("weight").toString().toInt();
            if (name.compare("base") != 0) {
                DataTypeStruct* type = DataTypeStruct::getType(name);
                type->setWeight(weight);

                averageWeight +=weight;
                valCount++;
            }
            reader.skipCurrentElement();
        }

        file.close();
    } else {
        cerr << "Could not open module weight file" << endl;
    }
    averageWeight /= valCount;
}

int AutomaticCoreAssigner::getRealObjectWeight(CObject* object, map<string, CObject*> objects) {
    CMethod* init = object->getInitMethod();
    list<CParameter>* params = init->getParameters();

    int weight = object->getType()->getWeight();
    if (weight == -1) {
        weight = averageWeight;
        string s = QString::fromUtf8(("WARNUNG: Kein Gewicht für Typ \"" + object->getType()->getName() + "\" gefunden! Wahrscheinlich ist die Datei \"module-weights.xml\" veraltet. Verwende Durchschnittswert.").c_str()).toStdString();
        cerr << s << endl;
        emit warningFound(s);
    }
    for (list<CParameter>::iterator i = ++params->begin(); i != params->end(); i++) {
        string value  = (*i).getValue();
        try {
            CObject* paramObject = objects.at(value);

            weight += getRealObjectWeight(paramObject, objects);
        } catch (exception) {
        }
    }
    return weight;
}

map<CObject*, int> AutomaticCoreAssigner::getRealDatastreamModuleWeights(DataLogger* dataLogger, int& totalWeight) {
    map<string, CObject*> objects = dataLogger->getObjectsMap();
    map<CObject*, int> result;

    list<DatastreamObject*> datastreamModules = dataLogger->getDatastreamModules();
    for (list<DatastreamObject*>::iterator it = datastreamModules.begin(); it != datastreamModules.end(); it++) {
        int weight = getRealObjectWeight(*it, objects);
        result[*it] = weight;
        totalWeight += weight;
        cout << "weight of " << (*it)->getName() << ": " << to_string(weight) << endl; //TODO
    }

    return result;
}

DatastreamObject* AutomaticCoreAssigner::findNextObject(list<DatastreamObject*> currentSet, std::unordered_set<DatastreamObject*>& done, list<DatastreamObject*>& queue, map<DatastreamObject*, map<DatastreamObject*, int>>& connections) {
    int worst = 0;
    DatastreamObject* nextObject = queue.front();

    for (list<DatastreamObject*>::iterator it = currentSet.begin(); it != currentSet.end(); it++) {
        DatastreamObject* current = *it;
        for (map<DatastreamObject*, int>::iterator conIt = connections[current].begin(); conIt != connections[current].end(); conIt++) {
            if (done.find(conIt->first) == done.end()) {
              int value = conIt->second;
              if (value > worst) {
                  nextObject = conIt->first;
                  worst = value;
              }
            }
        }
    }

    queue.remove(nextObject);
    done.insert(nextObject);
    return nextObject;
}

float AutomaticCoreAssigner::evaluatePartition(map<int, list<DatastreamObject*>>& sets, map<DatastreamObject*, map<DatastreamObject*, int>>& connections, map<int, int>& setWeights, int totalWeight, int cores) {
    float weightDistribution = 1.0f;

    for (int curCore = 0; curCore < cores; curCore++) {
        float localWeightDistribution = cores*setWeights[curCore] / (float)totalWeight;
        weightDistribution *= localWeightDistribution;
    }

    int connectorWeight = 0;

    for (int curCore = 0; curCore < cores; curCore++) {
        list<DatastreamObject*> curSet = sets[curCore];
        for (list<DatastreamObject*>::iterator setIt = curSet.begin(); setIt != curSet.end(); setIt++) {
            DatastreamObject* dobject = *setIt;
            map<DatastreamObject*, int> con = connections[dobject];
            for (map<DatastreamObject*, int>::iterator conIt = con.begin(); conIt != con.end(); conIt++) {
                DatastreamObject* dest = conIt->first;
                int contains = false;
                for (list<DatastreamObject*>::iterator it = curSet.begin(); it != curSet.end(); it++) {
                    if (*it == dest) {
                        contains = true;
                        break;
                    }
                }
                if (!contains) {
                    connectorWeight += conIt->second;
                }
            }
        }
    }
    connectorWeight /= 2; //we count every connection twice

    cout << "found solution:" << endl;
    cout << "weight Distribution: " << to_string(weightDistribution) << endl;
    cout << "connector weight: " << to_string(connectorWeight) << endl;
    float totalScore = heuristic->calculateTotalScore(weightDistribution, connectorWeight);
    cout << "total weight: " << to_string(totalScore) << endl << endl;
    return totalScore;
}

map<int, list<DatastreamObject*>> AutomaticCoreAssigner::generatePartition(DataLogger* dataLogger, int cores, map<CObject*, int> weights, int weightPerCore, map<DatastreamObject*, map<DatastreamObject*, int>>& connections, int shuffle, int totalWeight, float& score) {
    list<DatastreamObject*> queue = dataLogger->getDatastreamModules();

    while (shuffle--) {
        queue.push_back(queue.front());
        queue.pop_front();
    }

    unordered_set<DatastreamObject*> done;
    map<int, list<DatastreamObject*>> sets;
    map<int, int> setWeights;

    for (int curCore = 0; curCore < cores; curCore++) {
        DatastreamObject* dobject = queue.front();
        queue.pop_front();
        done.insert(dobject);
        int weightSum = weights.at(dobject);
        sets[curCore].push_back(dobject);
        while (((weightSum < weightPerCore) || (curCore == cores-1)) && !queue.empty()) {
            dobject = findNextObject(sets[curCore], done, queue, connections);
            sets[curCore].push_back(dobject);
            weightSum += weights.at(dobject);
        }
        setWeights[curCore] = weightSum;
        if (queue.empty()) {
            break;
        }
    }
    score = evaluatePartition(sets, connections, setWeights, totalWeight, cores);
    return sets;
}

bool AutomaticCoreAssigner::assignCores(DataLogger* dataLogger, map<CObject*, int> weights, int totalWeight, map<DatastreamObject*, map<DatastreamObject*, int>>& connections, int cores) {
    int weightPerCore = totalWeight / cores;

    //if the total divided by the number of cores is larger than the totalWeight we have no chance to find a solution
    if (weightPerCore > maxWeightPerCore) {
        return false;
    }

    cout << "using " << to_string(cores) << " CPU core(s)" << endl;

    map<int, list<DatastreamObject*>> bestSets;
    float bestScore = numeric_limits<float>::max();
    for (unsigned int shuffle = 0; shuffle < dataLogger->getDatastreamModules().size(); shuffle++) {
      float score;
      map<int, list<DatastreamObject*>> sets = generatePartition(dataLogger, cores, weights, weightPerCore, connections, shuffle, totalWeight, score);
      if (score < bestScore) {
          bestSets = sets;
          bestScore = score;
      }
    }

    cout << "partition result: (score: " << to_string(bestScore) << ")" << endl;
    for (map<int, list<DatastreamObject*>>::iterator it = bestSets.begin(); it != bestSets.end(); it++) {
        list<DatastreamObject*> l = it->second;
        cout << "set " << to_string(it->first) << ": " << endl;
        for (list<DatastreamObject*>::iterator a = l.begin(); a != l.end(); a++) {
            (*a)->setSpartanMcCore(it->first);

            cout << "\t" << (*a)->getName() << endl;
        }
    }


    return true;
}

void AutomaticCoreAssigner::analyzeConnections(DataLogger* dataLogger, map<DatastreamObject*, map<DatastreamObject*, int>>& connections) {
    list<DatastreamObject*> datastreamModules = dataLogger->getDatastreamModules();
    for (list<DatastreamObject*>::iterator it = datastreamModules.begin(); it != datastreamModules.end(); it++) {
        DatastreamObject* current = *it;
        list<ControlPortOut*> coutPorts = current->getControlOutPorts();
        list<DataPortOut*> dout = current->getDataOutPorts();

        for (list<ControlPortOut*>::iterator coutIt = coutPorts.begin(); coutIt != coutPorts.end(); coutIt++) {
            PortOut* p = *coutIt;
            if (p->isConnected()) {
                DatastreamObject* dest = p->getDestination()->getParent();
                connections[current][dest] += controlStreamWeight;
                connections[dest][current] += controlStreamWeight;
            }
        }
        for (list<DataPortOut*>::iterator doutIt = dout.begin(); doutIt != dout.end(); doutIt++) {
            PortOut* p = *doutIt;
            if (p->isConnected()) {
                DatastreamObject* dest = p->getDestination()->getParent();
                connections[current][dest] += dataStreamWeight;
                connections[dest][current] += dataStreamWeight;
            }
        }
    }
}

bool AutomaticCoreAssigner::assignCores(DataLogger* dataLogger) {
    cout << endl << "starting automatic core assignment" << endl;

    int totalWeight = 0;
    map<CObject*, int> weights = getRealDatastreamModuleWeights(dataLogger, totalWeight);
    cout << "total weight of system: " << to_string(totalWeight) << endl << endl;

    map<DatastreamObject*, map<DatastreamObject*, int>> connections;
    analyzeConnections(dataLogger, connections);

    cout << "searching for partitioning solutions" << endl
         << "(weight Distribution should be close to 1)" << endl
         << "(connector weight should be small)" << endl
         << "(total weight should be small)" << endl;

    for (int cores = minCores; cores <= maxCores; cores++) {
        if (assignCores(dataLogger, weights, totalWeight, connections, cores)) {
            return true;
        }
    }
    return false;
}

QXmlStreamWriter& operator<<(QXmlStreamWriter& out, AutomaticCoreAssigner& aco) {
    out.writeStartElement("AutomaticCoreAssigner");

    out.writeAttribute("minCores", to_string(aco.minCores).c_str());
    out.writeAttribute("maxCores", to_string(aco.maxCores).c_str());
    out.writeAttribute("maxWeightPerCore", to_string(aco.maxWeightPerCore).c_str());
    out.writeAttribute("dataStreamWeight", to_string(aco.dataStreamWeight).c_str());
    out.writeAttribute("controlStreamWeight", to_string(aco.controlStreamWeight).c_str());

    out.writeEndElement();
    return out;
}

QXmlStreamReader& operator>>(QXmlStreamReader& in, AutomaticCoreAssigner& aco) {
    const QXmlStreamAttributes& a = in.attributes();
    aco.minCores = a.value("minCores").toString().toInt();
    aco.maxCores = a.value("maxCores").toString().toInt();
    aco.maxWeightPerCore = a.value("maxWeightPerCore").toString().toInt();
    aco.dataStreamWeight = a.value("dataStreamWeight").toString().toInt();
    aco.controlStreamWeight = a.value("controlStreamWeight").toString().toInt();

    in.skipCurrentElement();
    return in;
}
