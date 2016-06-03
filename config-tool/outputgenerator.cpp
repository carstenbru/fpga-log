#include "outputgenerator.h"
#include <list>
#include <string>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <QCoreApplication>
#include <QDirIterator>
#include "cobject.h"
#include "datalogger.h"

using namespace std;

OutputGenerator::OutputGenerator(DataLogger *dataLogger, string directory, bool changeOtherModulesCodeID) :
    dataLogger(new DataLogger(*dataLogger)),
    directory(directory),
    usedIdCounter(0),
    process(this),
    busy(false),
    error(false),
    pcPeripheralIdCounter(0),
    pcPeripheralCompareCounter(0),
    pcPeripheralTimerCounter(0),
    timestampInvertMask(0),
    optimizeBlockRAMcount(OPTIMIZE_BLOCK_RAMS),
    firmwareSizeMeasurement(false)
{
    process.setWorkingDirectory(directory.c_str());
    connect(&process, SIGNAL(readyReadStandardOutput()), this, SLOT(newChildStdOut()));
    connect(&process, SIGNAL(readyReadStandardError()), this, SLOT(newChildErrOut()));
    connect(&process, SIGNAL(finished(int)), this, SLOT(processFinished()));

    // reset core_IDs of objects
    if (changeOtherModulesCodeID) {
        vector<CObject*> objects = this->dataLogger->getOtherObjects();
        for (vector<CObject*>::iterator it = objects.begin(); it != objects.end(); it++) {
            (*it)->setSpartanMcCore(-1);
        }
    }

    calculateUsedProcessorIDs();
    copyProjectTemplate();
}

OutputGenerator::~OutputGenerator() {
    delete dataLogger;
}

void OutputGenerator::copyProjectTemplate() {
    string path = "../config-tool-files/template/";
    QDirIterator dirIter(QString(path.c_str()), QDirIterator::Subdirectories);
    while (dirIter.hasNext()) {
        dirIter.next();
        if (QFileInfo(dirIter.filePath()).isFile()) {
            QString newPath = dirIter.filePath();
            newPath.remove(0, path.length());
            newPath = (directory + "/").c_str() + newPath;

            if (dirIter.filePath().contains("firmware")) {
                for (set<int>::iterator i = processorSet.begin(); i != processorSet.end(); i++) {
                    QString newPathSubst = newPath;
                    newPathSubst.insert(newPath.indexOf("firmware")+8, QString("_") + to_string(*i).c_str());

                    QDir().mkpath(QFileInfo(newPathSubst).dir().absolutePath());
                    if (dirIter.fileName().compare("main.c") == 0) {
                        copyMainC(dirIter.filePath(), newPathSubst);
                    } else {
                        QFile::copy(dirIter.filePath(), newPathSubst);
                    }
                }
            } else {
                QDir().mkpath(QFileInfo(newPath).dir().absolutePath());
                QFile::copy(dirIter.filePath(), newPath);
            }
        }
    }
}

void OutputGenerator::copyMainC(QString src, QString dest) {
    ofstream destFile;
    destFile.open(dest.toStdString());

    ifstream srcFile;
    srcFile.open(src.toStdString());

    char buf[1024];
    while (srcFile.getline(buf, 1024)) {
        if ((buf[0] == 0) || (strstr("CLOCK_FREQUENCY_DEFINITION", buf) == NULL)) {
            destFile << buf << endl;
        } else {
            destFile << "#define CLOCK_FREQUENCY " << dataLogger->getPeriClk() << endl;
        }
    }
}

void OutputGenerator::exec(string cmd) {
    if (!busy) {
        busy = true;
        process.start(QString(cmd.c_str()));
    } else {
        pending.push_back(cmd);
    }
}

void OutputGenerator::processFinished() {
    if (pending.empty() || error) {
        busy = false;
        emit finished(error || timingError, timingError);
    } else {
        string next = pending.front();
        pending.pop_front();
        if (next.compare("generateSystemXML(false)") != 0) {
            if (next.find("spmc-size") != string::npos) {
                firmwareSizeMeasurement = true;
            }
            process.start(QString(next.c_str()));
        } else {
            generateSystemXML(false);
            processFinished();
        }
    }
}

void OutputGenerator::checkSynthesisMessage(string message) {
    if (message.find("System synthesis complete.") != string::npos) {
        synthesisSuccessful = true;
    }
    if (message.find("Timing: Completed") != string::npos) {
        if (message.find("Timing: Completed - No errors found.") == string::npos) {
            timingError = true;
            emit errorFound("Taktfrequenz zu hoch");
        }
    }
}

void OutputGenerator::checkFirmwareSizeMessage(string message) {
    firmwareSizeMeasurement = false;

    QRegExp rx("[ ]");// match a space
    QStringList list = QString(message.c_str()).split(rx, QString::SkipEmptyParts);
    emit firmwareSizeResult(list.at(8).toInt());
}

void OutputGenerator::newChildStdOut() {
    string s = QString(process.readAllStandardOutput()).toStdString();
    cout << s;
    checkSynthesisMessage(s);
    if (firmwareSizeMeasurement) {
        checkFirmwareSizeMessage(s);
    }
}

void OutputGenerator::newChildErrOut() {
    string s = QString(process.readAllStandardError()).toStdString();
    cerr << s;
}

void OutputGenerator::measureFirmwareSize() {
    optimizeBlockRAMcount = false;
    generateConfigFiles();
    exec("make firmware");

    string elf = QFileInfo((directory + "/build/firmware/subsystem_0/main.elf").c_str()).dir().absolutePath().toStdString();
    string spmc_root = QProcessEnvironment::systemEnvironment().value("SPARTANMC_ROOT").toStdString();
    exec(spmc_root + "/bin/spmc-size " + elf + "/main.elf");
}

void OutputGenerator::generateConfigFiles() {
    timingError = false;
    synthesisSuccessful = false;

    dataLogger->addCoreConnectors();
    generateSystemXML(true); //re-generate System-XML with large BlockRam counts
    generateCSources();

    exec("make jconfig +args=\"--generate system.xml\"");

    if (optimizeBlockRAMcount) {
        exec("make constraints"); //generate contraints file to see neccessary BlockRAM count in each subsystem
        exec("generateSystemXML(false)"); //re-generate System-XML with real BlockRam counts,
                                          //we use our asynchronous "exec" function so it will be executed after the previous task only
        exec("make jconfig +args=\"--generate system.xml\""); //update generated files
    }
}

void OutputGenerator::synthesizeSystem() {
    timingError = false;
    generateConfigFiles();

    //exec("make all"); //TODO
}

void OutputGenerator::synthesizeOnly() {
    timingError = false;
    synthesisSuccessful = false;

    exec("make all");
}

void OutputGenerator::generateCSource(int subsystemID) {
    pcPeripheralIdCounter = 0;
    pcPeripheralsStream.clear();
    pcPeripheralsStream.str("");
    pcPeripheralCompareCounter = 0;
    pcPeripheralTimerCounter = 0;

    string firmwareFolder = "/firmware_" + to_string(subsystemID);
    ofstream file;
    file.open(directory + firmwareFolder + "/src/logger_config.c");

    ostream& stream = file;

    writePreamble(stream);
    stream << endl;

    stringstream tmpFile;
    stringstream tmpInit;

    //we need to do the initialization first as it sets core-IDs of objects (not datastream modules)
    writeInitFunction(tmpInit, subsystemID);
    tmpInit << endl;

    writeVariableDefinitions(tmpFile, subsystemID);
    tmpFile << endl << tmpInit.str();
    writeConnectPorts(tmpFile, subsystemID);
    tmpFile << endl;
    writeAdvancedConfig(tmpFile, subsystemID);

    writeHeaderIncludes(stream);
    stream << endl << tmpFile.str();

    file.close();

    file.open(directory + firmwareFolder + "/include/pc_peripherals.h");
    writePreamble(file);
    file << endl << "#ifndef PERIPHERALS_SIM_H_" << endl << "#define PERIPHERALS_SIM_H_" << endl << endl;
    file << "#define PIPE_COUNT " << pcPeripheralIdCounter << endl << endl;
    file << "#define TIMER_COUNT " << pcPeripheralTimerCounter << endl;
    file << "#define COMPARE_COUNT " << pcPeripheralCompareCounter << endl << endl;
    file << pcPeripheralsStream.str() << endl;
    file << "#define TIMESTAMP_CAPTURE_SIGNALS_COUNT " << usedTimestampSources[subsystemID] << endl;
    string tscs = pcTimestampCaptureStream[subsystemID].str();
    tscs.erase(0, 2);
    file << "#define TIMESTAMP_CAPTURE_SIGNALS {" << tscs << "}" << endl;
    file << endl << "#endif" << endl;
    file.close();
}

void OutputGenerator::generateCSources() {
    usedHeaders.clear();
    determineHeaders();

    for (set<int>::iterator i = processorSet.begin(); i != processorSet.end(); i++) {
        generateCSource(*i);
    }

    vector<CObject*> objects = dataLogger->getOtherObjects();
    for (vector<CObject*>::iterator it = objects.begin(); it != objects.end(); it++) {
        if ((*it)->getSpartanMcCore() == -1) {
            string s = "Objekt " + (*it)->getName() + " nicht verwendet";
            cerr << "WARNUNG: " << s << "!" << endl;
            emit errorFound(s);
        }
    }

    cout << "C-Konfigurationsdateien erfolgreich geschrieben." << endl;
}

void OutputGenerator::determineHeaders() {
    map<string, CObject*> objects = dataLogger->getObjectsMap();

    for (map<string, CObject *>::iterator i = objects.begin(); i != objects.end(); i++) {
        CObject* object = i->second;
        putUsedHeader(object->getType()->getHeaderName(), object->getType()->isGlobal());
        putUsedHeader(object->getInitMethod()->getHeaderName(), object->getType()->isGlobal());

        list<CMethod*> methods = object->getAdvancedConfig();
        for (list<CMethod*>::iterator mIt = methods.begin(); mIt != methods.end(); mIt++) {
            CMethod* method = *mIt;
            putUsedHeader(method->getHeaderName(), object->getType()->isGlobal());
        }
    }
}

void OutputGenerator::writeVariableDefinitions(std::ostream& stream, int subsystemID) {
    map<string, CObject*> objects = dataLogger->getObjectsMap();

    for (map<string, CObject *>::iterator i = objects.begin(); i != objects.end(); i++) {
        if (i->second->getSpartanMcCore() == subsystemID) {
          //putUsedHeader(i->second->getType()->getHeaderName(), i->second->getType()->isGlobal());
          stream << i->second->getType()->getName() << "\t" << i->first << ";" << endl;
        }
    }
}

void OutputGenerator::writeInitFunction(ostream& stream, int subsystemID) {
    map<string, CObject*> objects = dataLogger->getObjectsMap();

    stream << "void init_objects(void) {" << endl;

    map<string, bool> initDone;
    for (map<string, CObject *>::iterator i = objects.begin(); i != objects.end(); i++) {
        if (i->second->getSpartanMcCore() == subsystemID) {
            writeObjectInit(stream, i->second, objects, initDone);
        }
    }

    stream << "}" << endl;
}

void writeCast(std::ostream& stream, CParameter* signatureParameter) {
    stream << "(" << signatureParameter->getDataType()->getName();
    if (signatureParameter->isPointer())
        stream << "*";
    stream << ")";
}

void writeCastIfNeccessary(std::ostream& stream, CParameter* signatureParameter, DataType* actualType) {
    if (!signatureParameter->getDataType()->equals(actualType)) {
        writeCast(stream, signatureParameter);
    }
}

void OutputGenerator::writeObjectInit(std::ostream& stream, CObject* object, std::map<string, CObject *>& objects, map<string, bool>& initDone) {
    if (!initDone[object->getName()]) {
        stringstream tmpStream;

        CMethod* init = object->getInitMethod();
        //putUsedHeader(init->getHeaderName(), object->getType()->isGlobal());
        list<CParameter>* params = init->getParameters();
        tmpStream << "  " << object->getType()->getCleanedName() << "_"
                  << init->getName() << "(&" << object->getName();
        for (list<CParameter>::iterator i = ++params->begin(); i != params->end(); i++) {
            tmpStream << ", ";

            string value  = (*i).getValue();
            try {
                CObject* paramObject = objects.at(value);
                writeCastIfNeccessary(tmpStream,  &(*i), paramObject->getType());
                if ((*i).isPointer())
                  tmpStream << "&";
                if (!initDone[value]) {
                    int coreId = object->getSpartanMcCore();
                    int oldCoreId = paramObject->getSpartanMcCore();
                    if ((oldCoreId != -1) && (oldCoreId != coreId)) {
                        error = true;
                        string s = "Objekt " + paramObject->getName() + " auf mehreren SpartanMC Cores verwendet";
                        cerr << "FEHLER: " << s << "!" << endl;
                        emit errorFound(s);
                    }
                    paramObject->setSpartanMcCore(coreId);
                    writeObjectInit(stream, paramObject, objects, initDone);
                }
            } catch (exception) {
                if ((*i).getDataType()->hasSuffix("_regs_t") || (*i).getDataType()->hasSuffix("_dma_t")) {
                    writeCast(tmpStream, &(*i));
                    value = object->getName() + "_" + (*i).getName();
                    transform(value.begin(), value.end(), value.begin(), ::toupper);
                    definePeripheralForSimulation(value, (*i).getDataType());
                } else {
                    value = (*i).getCValue();
                }
            }

            if (value.empty()) {
                error = true;
                string s = "Parameter " + (*i).getName() + " von Objekt " + object->getName() + " nicht gesetzt";
                cerr << "FEHLER: " << s << "!" << endl;
                emit errorFound(s);
            }

            tmpStream << value;
        }
        stream << tmpStream.str() << ");" << endl;
        initDone[object->getName()] = true;
    }
}

void OutputGenerator::writeConnectPorts(ostream& stream, int subsystemID) {
    stream << "void connect_ports(void) {" << endl;

    list<DatastreamObject*> modules = dataLogger->getDatastreamModules();
    for (list<DatastreamObject*>::iterator i = modules.begin(); i != modules.end(); i++) {
        if ((*i)->getSpartanMcCore() == subsystemID) {
            DatastreamObject* module = *i;

            list<PortOut*> ports = module->getOutPorts(PORT_TYPE_DATA_OUT);
            list<PortOut*> portsC = module->getOutPorts(PORT_TYPE_CONTROL_OUT);
            ports.insert(ports.end(), portsC.begin(), portsC.end());

            for (list<PortOut*>::iterator portIt = ports.begin(); portIt != ports.end(); portIt++) {
                PortOut* port = *portIt;
                if (port->isConnected()) {
                    Port* destinationPort = port->getDestination();
                    DatastreamObject* destinationModule = destinationPort->getParent();

                    stream << "  " << module->getType()->getCleanedName();
                    if (!port->isMultiPort())
                        stream << "_set_";
                    else
                        stream << "_add_";
                    stream << port->getName() << "(";
                    stream << "&" << module->getName() << ", ";
                    stream << destinationModule->getType()->getCleanedName() << "_get_" << destinationPort->getName();
                    stream << "(&" << destinationModule->getName() << ")";
                    stream << ");" << endl;
                }
            }
        }
    }

    stream << "}" << endl;
}

void OutputGenerator::writeHeaderIncludes(std::ostream& stream) {
    stream << "#include \"logger_config.h\"" << endl;
    stream << "#include <system/peripherals.h>" << endl;
    stream << "#include <fpga-log/pc_compatibility.h>" << endl;
    stream << "#include <fpga-log/control_port.h>" << endl << endl;
    for (set<string>::iterator i = usedHeaders.begin(); i != usedHeaders.end(); i++) {
        stream << "#include " << *i << endl;
    }
}

void OutputGenerator::writePreamble(std::ostream& stream) {
    stream << "/*" << endl;
    stream << " * This file was auto-generated by the config-tool of fpga-log." << endl;
    stream << " * Do not change this file!" << endl;
    stream << " */" << endl;
}

void OutputGenerator::putUsedHeader(std::string headerName, bool global) {
    headerName.erase(0, headerName.find("include") + 8);
    if (global) {
        usedHeaders.insert("<fpga-log/" + headerName + ">");
    } else {
        usedHeaders.insert("\"" + headerName + "\"");
    }
}

void OutputGenerator::writeMethod(std::ostream& stream, CObject* object, CMethod* method, map<string, CObject *>& objects) {
    //putUsedHeader(method->getHeaderName(), object->getType()->isGlobal());
    list<CParameter>* params = method->getParameters();
    string methodName = method->getCompleteName();
    if (methodName.empty()) {
        methodName = object->getType()->getCleanedName() + "_" + method->getName();
    }
    stream << "  " << methodName << "(&" << object->getName();
    for (list<CParameter>::iterator i = ++params->begin(); i != params->end(); i++) {
        stream << ", ";

        try {
            objects.at((*i).getValue());
            stream << "&";
        } catch (exception) {
        }
        string value  = (*i).getCValue();

        if (value.empty()) {
            error = true;
            string s = "Parameter " + (*i).getName() + " von Objekt " + object->getName() + " nicht gesetzt";
            cerr << "FEHLER: " << s << "!" << endl;
            emit errorFound(s);
        }

        stream << value;
    }
    stream << ");" << endl;
}

void OutputGenerator::writeAdvancedConfig(std::ostream& stream, int subsystemID) {
    stream << "void advanced_config(void) {" << endl;

    map<string, CObject*> objects = dataLogger->getObjectsMap();

    for (map<string, CObject *>::iterator i = objects.begin(); i != objects.end(); i++) {
        if (i->second->getSpartanMcCore() == subsystemID) {
            CObject* object = i->second;
            list<CMethod*> methods = object->getAdvancedConfig();
            for (list<CMethod*>::iterator mIt = methods.begin(); mIt != methods.end(); mIt++) {
                writeMethod(stream, object, *mIt, objects);
            }
        }
    }

    stream << "}" << endl;
}

void OutputGenerator::writeTimestampGen(QXmlStreamWriter& writer,string subsystemID, int subsystemNumber) {
    usedTimestampSources[subsystemNumber] = max(1, usedTimestampSources[subsystemNumber]);
    SpmcPeripheral timestampGen("TIMESTAMP_GEN", DataType::getType("timestamp_gen_regs_t"), NULL, dataLogger);
    timestampGen.getParameter("SOURCES")->setValue(to_string(usedTimestampSources[subsystemNumber]).c_str());
    timestampGen.getParameter("PIN_SOURCES")->setValue(to_string(max(1, usedTimestampPinSources[subsystemNumber])).c_str());
    timestampGen.getParameter("INVERTED_SOURCES_MASK")->setValue(to_string(timestampInvertMask).c_str());

    addTimestampPinConnections(timestampGen.getPort("Timestamp capture sources", "pin_source"), subsystemNumber);

    writePeripheral(writer, &timestampGen, subsystemID, subsystemNumber);
}

void OutputGenerator::writeClkConnection(std::ostream& stream, std::string destination) {
    QString clkConnection;
    QXmlStreamWriter clkConnectionWriter(&clkConnection);
    clkConnectionWriter.setAutoFormatting(true);

    writeConnection(clkConnectionWriter, "SYSCLK/#PORT." + destination, 0);

    stream << clkConnection.toStdString() << endl;
}

void OutputGenerator::generateSpmcSubsystem(ostream& stream, int id, bool useMaxBlockRAMs) {
    usedTimestampSources[id] = 0;
    usedTimestampPinSources[id] = 0;
    timestampInvertMask = 0;

    ifstream subTemplateFile("../config-tool-files/template_subsystem.xml");
    if (!subTemplateFile.is_open()) {
        string s = "Subsystem Template XML konnte nicht geöffnet werden.";
        cerr << s << endl;
        emit errorFound(s);
        return;
    }

    string subsystemName = "SUBSYSTEM_" + to_string(id);

    while (!subTemplateFile.eof()) {
        string line;
        getline(subTemplateFile, line);
         if (line.compare("FPGA-LOG_PERIPHERALS") == 0) {
            QString peripherals;
            QXmlStreamWriter peripheralsWriter(&peripherals);
            peripheralsWriter.setAutoFormatting(true);
            writePeripherals(peripheralsWriter, subsystemName, id);
            if ((usedTimestampPinSources[id] > 0) || (usedTimestampSources[id] > 0)) {
                writeTimestampGen(peripheralsWriter, subsystemName, id);
            }

            stream << peripherals.toStdString() << endl;
        } else if (line.compare("SUBSYSTEM_HEADER") == 0) {
            stream << "<processor_module id=\""<< subsystemName << "\">" << endl;
        } else if (line.compare("FIRMWARE_LOC") == 0) {
             stream << "<attribute id=\"firmware\" xml:space=\"preserve\">./firmware_" << id << "</attribute>" << endl;
        } else if (line.compare("SUBSYSTEM_NAME") == 0) {
             stream << "<attribute id=\"name\" xml:space=\"preserve\">subsystem_"<< id << "</attribute>" << endl;
        } else if (line.compare("CONNECTION_CLK1") == 0) {
             writeClkConnection(stream, "clk1");
        } else if (line.compare("CONNECTION_CLK2") == 0) {
             writeClkConnection(stream, "clk2");
        } else if (line.compare("CONNECTION_CLK2X") == 0) {
             writeClkConnection(stream, "clk2x");
        } else if (line.compare("CONNECTION_LOCKED") == 0) {
             writeClkConnection(stream, "locked");
        } else if (line.compare("CONNECTION_PHASE") == 0) {
             writeClkConnection(stream, "phase");
        } else if (line.compare("RAMBLOCKS_PARAM") == 0) {
             QString bparamString;
             QXmlStreamWriter bparamWriter(&bparamString);
             bparamWriter.setAutoFormatting(true);

             CParameter bparam("RAMBLOCKS", DataTypeNumber::getType("int"));
             if (useMaxBlockRAMs) {
                 bparam.setValue(to_string(MAX_BLOCK_RAMS));
             } else {
                 bparam.setValue(to_string(determineMinBlockRAMcount(id)));
             }
             writeParameter(bparamWriter, &bparam);

             stream << bparamString.toStdString() << endl;
        } else {
            stream << line << endl;
        }
    }
    subTemplateFile.close();
}

void OutputGenerator::calculateUsedProcessorIDs() {
    processorSet.clear();
    map<string, CObject*> objects = dataLogger->getObjectsMap();
    for (map<string, CObject*>::iterator i = objects.begin(); i != objects.end(); i++) {
        int id = i->second->getSpartanMcCore();
        if (id >= 0) {
            processorSet.insert(id);
        }
    }
    if (processorSet.empty()) {
        string s = "WARNUNG: Kein Prozessor verwendet. Sehr wahrscheinlich ist der Datenlogger leer!";
        cerr << s << endl;
        emit errorFound(s);

        processorSet.insert(0);
    }
}

void OutputGenerator::generateSystemXML(bool useMaxBlockRAMs) {
    ifstream templateFile("../config-tool-files/template.xml");
    if (!templateFile.is_open()) {
        string s = "System Template XML konnte nicht geöffnet werden.";
        cerr << s << endl;
        emit errorFound(s);
        return;
    }

    ofstream file;
    file.open(directory + "/system.xml");
    ostream& stream = file;

    QString targetNode;
    QXmlStreamWriter targetWriter(&targetNode);
    targetWriter.setAutoFormatting(true);
    writeTargetNode(targetWriter);

    while (!templateFile.eof()) {
        string line;
        getline(templateFile, line);
        if (line.compare("TARGET_NODE") == 0) {
            stream << targetNode.toStdString() << endl;
        } else if (line.compare("CLOCK_PERIOD_ATTRIBUTE") == 0) {
            stream << "<attribute id=\"value\">" << (1000000000.0f / dataLogger->getClk()) << "</attribute>" << endl;
        } else if (line.compare("SYSTEM_CLOCK_PERIOD_ATTRIBUTE") == 0) {
            stream << "<attribute id=\"value\">" << (1000000000.0f / dataLogger->getSystemClk()) << "</attribute>" << endl;
        } else if (line.compare("CLOCK_DIVIDE_ATTRIBUTE") == 0) {
            stream << "<attribute id=\"value\">" << dataLogger->getClkDivide() << "</attribute>" << endl;
        } else if (line.compare("CLOCK_MULTIPLY_ATTRIBUTE") == 0) {
            stream << "<attribute id=\"value\">" << dataLogger->getClkMultiply() << "</attribute>" << endl;
        } else if (line.compare("2000HZ_DIV_VALUE_ATTRIBUTE") == 0) {
            stream << "<attribute id=\"value\">" << (dataLogger->getPeriClk() / 2000) << "</attribute>" << endl;
        } else if (line.compare("SUBSYSTEMS") == 0) {
            for (set<int>::iterator i = processorSet.begin(); i != processorSet.end(); i++) {
                generateSpmcSubsystem(file, *i, useMaxBlockRAMs);
            }
        } else if (line.compare("FPGA_PINS") == 0) {
            QString pins;
            QXmlStreamWriter pinsWriter(&pins);
            pinsWriter.setAutoFormatting(true);
            writePins(pinsWriter);
            stream << pins.toStdString() << endl;
        } else {
            stream << line << endl;
        }
    }
    file.close();
    cout << "System Konfigurationsdatei erfolgreich geschrieben." << endl;
}

void OutputGenerator::writeAttributeElement(QXmlStreamWriter& writer, QString id, QString text) {
    writer.writeStartElement("attribute");
    writer.writeAttribute("id", id);
    writer.writeCharacters(text);
    writer.writeEndElement();
}

void OutputGenerator::writeTargetNode(QXmlStreamWriter& writer) {
    writer.writeStartElement("node");
    writer.writeAttribute("id", "TARGET");
    writeAttributeElement(writer, "preset", dataLogger->getTarget()->getValue().c_str());
    writer.writeEndElement();
}

void OutputGenerator::writeParameter(QXmlStreamWriter& writer, CParameter* parameter) {
    writer.writeStartElement("parameter");
    string name = parameter->getName();
    string pname = name;
    transform(pname.begin(), pname.end(), pname.begin(), ::toupper);
    pname = "#PARAM." + pname;
    writer.writeAttribute("id", pname.c_str());
    writeAttributeElement(writer, "name", name.c_str());
    if (name.compare("CLOCK_FREQUENCY") == 0) {
        writeAttributeElement(writer, "value", to_string(dataLogger->getPeriClk()).c_str());
    } else {
        writeAttributeElement(writer, "value", parameter->getValue().c_str());
    }
    writer.writeEndElement();
}

void OutputGenerator::writePeripheral(QXmlStreamWriter& writer, SpmcPeripheral* peripheral, string subsystemID, int subsystemNumber) {
    writer.writeStartElement("peripheral");
    string peripheralName = peripheral->getCompleteName().c_str();
    std::transform(peripheralName.begin(), peripheralName.end(), peripheralName.begin(), ::tolower);

    string peripheralNameUpper = peripheralName;
    transform(peripheralNameUpper.begin(), peripheralNameUpper.end(), peripheralNameUpper.begin(), ::toupper);
    writer.writeAttribute("id", peripheralNameUpper.c_str());
    string type = peripheral->getDataType()->getCleanedName();
    type.erase(type.length() - 5, type.length());
    writeAttributeElement(writer, "module_type", type.c_str());
    writeAttributeElement(writer, "name", peripheralName.c_str());

    list<CParameter*> parameters = peripheral->getParameters();
    for (list<CParameter*>::iterator i = parameters.begin(); i != parameters.end(); i++) {
        writeParameter(writer, *i);
    }

    map<string, list<PeripheralPort*> > ports = peripheral->getPorts();
    for (map<string, list<PeripheralPort*> >::iterator groupIt = ports.begin(); groupIt != ports.end(); groupIt++) {
        list<PeripheralPort*> group = groupIt->second;
        for (list<PeripheralPort*>::iterator i = group.begin(); i != group.end(); i++) {
            PeripheralPort* port = *i;
            writer.writeStartElement("port");
            string portName = port->getName();
            string portId = "#PORT." + portName;
            writer.writeAttribute("id", portId.c_str());

            writeAttributeElement(writer, "name", portName.c_str());

            list<CParameter*> lines = port->getLines();
            int lsb = 0;
            for (list<CParameter*>::iterator portIt = lines.begin(); portIt != lines.end(); portIt++) {
                QString destination = (*portIt)->getValue().c_str();
                if (!destination.isEmpty()) {
                    if (destination.contains(":")) { //pin destination
                        destination.replace(":", "_");
                        usedPins.push_back(FpgaPin(destination.toStdString(), port->getDirection(), port->getConstraints()));
                        destination = "#PIN." + destination;
                    } else if (destination.startsWith("./")) { //other peripheral of this module as destination
                        destination.remove(0, 2);
                        string destName = destination.toStdString();
                        int slash = destName.find("/");
                        destName.erase(slash, destName.length());
                        destName = subsystemID + "/" + peripheral->getParentName() + "_" + destName;
                        transform(destName.begin(), destName.end(), destName.begin(), ::toupper);
                        destination.remove(0, slash);
                        destination = destName.c_str() + destination;
                    } else if (destination.startsWith("../")) { //other entity of this subsystem as destination
                        destination.remove(0, 3);
                        string destName = destination.toStdString();
                        int slash = destName.find("/");
                        destName.erase(slash, destName.length());
                        destName = subsystemID + "/" + destName;
                        transform(destName.begin(), destName.end(), destName.begin(), ::toupper);
                        destination.remove(0, slash);
                        destination = destName.c_str() + destination;
                    }else if (destination.compare("TIMESTAMP_GEN") == 0) { //timestamp generator source
                        destination = (subsystemID + "/TIMESTAMP_GEN/#PORT.internal_source").c_str();
                        string param = (*portIt)->getName();
                        peripheral->getParentObject()->getInitMethod()->getParameter(param)->setValue(to_string(++usedTimestampSources[subsystemNumber]));
                        pcTimestampCaptureStream[subsystemNumber] << ", " << peripheralNameUpper;
                    }
                    writeConnection(writer, destination.toStdString(), lsb);
                } else {
                    if (!(*portIt)->getHideFromUser()) {
                        if ((peripheral->getParentName().find("_connector") == string::npos) &&
                                (!peripheral->getParentName().empty())) {
                            emit errorFound("Pin " + portName + " (" + groupIt->first + ") im Modul " + peripheral->getParentName() + " nicht zugewiesen");
                        }
                    }
                }
            }

            writer.writeEndElement();
        }
    }

    writeSpmcConnections(writer, subsystemID);
    if (peripheral->hasDMA()) {
        writeDMAConnections(writer, subsystemID);
    }

    writer.writeEndElement();
}

void OutputGenerator::writePeripherals(QXmlStreamWriter& writer, string subsystemID, int subsystemNumber) {
    map<string, CObject*> objects = dataLogger->getObjectsMap();
    for (map<string, CObject*>::iterator i = objects.begin(); i != objects.end(); i++) {
        if ((i->second->getSpartanMcCore() == subsystemNumber)) {
            list<SpmcPeripheral*> peripherals = i->second->getPeripherals();
            for (list<SpmcPeripheral*>::iterator ip = peripherals.begin(); ip != peripherals.end(); ip++) {
                writePeripheral(writer, *ip, subsystemID, subsystemNumber);
            }
        }
    }
}

void OutputGenerator::writeConnection(QXmlStreamWriter& writer, std::string destination, int lsb) {
    writer.writeStartElement("connection");
    string connectionId = "#" + to_string(usedIdCounter++);
    writer.writeAttribute("id", connectionId.c_str());
    destination = "/TOP_MODULE/" + destination;
    writeAttributeElement(writer, "connected_port", destination.c_str());
    writeAttributeElement(writer, "lsb_index", to_string(lsb).c_str());

    writer.writeEndElement();
}

void OutputGenerator::writePortConnection(QXmlStreamWriter& writer, std::string port, string destination, int lsb) {
    writer.writeStartElement("port");
    string portId = "#PORT." + port;
    writer.writeAttribute("id", portId.c_str());
    writeAttributeElement(writer, "name", port.c_str());

    writeConnection(writer, destination, lsb);

    writer.writeEndElement();
}

void OutputGenerator::writeSpmcConnections(QXmlStreamWriter& writer, string subsystemID) {
    writePortConnection(writer, "access_peri", subsystemID + "/SPARTANMC/#PORT.access_peri", 0);
    writePortConnection(writer, "addr_peri", subsystemID + "/SPARTANMC/#PORT.addr_peri", 0);
    writePortConnection(writer, "clk_peri", subsystemID + "/SPARTANMC/#PORT.clk_peri", 0);
    writePortConnection(writer, "do_peri", subsystemID + "/SPARTANMC/#PORT.do_peri", 0);
    writePortConnection(writer, "reset", subsystemID + "/SPARTANMC/#PORT.reset", 0);
    writePortConnection(writer, "wr_peri", subsystemID + "/SPARTANMC/#PORT.wr_peri", 0);
    writePortConnection(writer, "di_peri", subsystemID + "/SPARTANMC/#PORT.di_peri", 0);
}

void OutputGenerator::writeDMAConnections(QXmlStreamWriter& writer, string subsystemID) {
    writePortConnection(writer, "addr_high", subsystemID + "/SPARTANMC/#PORT.addr_high", 0);
    writePortConnection(writer, "mem_access", subsystemID + "/SPARTANMC/#PORT.mem_access", 0);
    writePortConnection(writer, "mem_clk", subsystemID + "/SPARTANMC/#PORT.mem_clk", 0);
    writePortConnection(writer, "store_access", subsystemID + "/SPARTANMC/#PORT.store_access", 0);
    writePortConnection(writer, "store_access_high", subsystemID + "/SPARTANMC/#PORT.store_access_high", 0);
    writePortConnection(writer, "store_access_low", subsystemID + "/SPARTANMC/#PORT.store_access_low", 0);
}

void OutputGenerator::writeClkPin(QXmlStreamWriter& writer) {
    writer.writeStartElement("fpga_pin");
    CParameter* clkPinParam = dataLogger->getClockPin();
    QString pinName = clkPinParam->getValue().c_str();
    pinName.replace("_", ":");
    string group = Pin::getGroupFromFullName(pinName.toStdString());
    Pin* pin = DataTypePin::getPinType()->getPin(group, Pin::getPinFromFullName(pinName.toStdString()));
    pinName.replace(":", "_");
    writer.writeAttribute("id", "#PIN.CLK");

    writeAttributeElement(writer, "name", pinName);
    writeAttributeElement(writer, "group", group.c_str());
    writeAttributeElement(writer, "clock_freq", to_string(dataLogger->getClk()).c_str());
    writeAttributeElement(writer, "direction", "INPUT");
    writeAttributeElement(writer, "io_standard", "LVCMOS33");
    if (pin != NULL) {
      writeAttributeElement(writer, "loc", pin->getLoc().c_str());
    }

    writer.writeEndElement();
}

void OutputGenerator::writePins(QXmlStreamWriter& writer) {
    writeClkPin(writer);

    for (list<FpgaPin>::iterator i = usedPins.begin(); i != usedPins.end(); i++) {
         writer.writeStartElement("fpga_pin");
         QString pinName = (*i).getName().c_str();
         pinName.replace("_", ":");
         string group = Pin::getGroupFromFullName(pinName.toStdString());
         Pin* pin = DataTypePin::getPinType()->getPin(group, Pin::getPinFromFullName(pinName.toStdString()));
         pinName.replace(":", "_");
         string pinId = "#PIN." + pinName.toStdString();
         writer.writeAttribute("id", pinId.c_str());

         writeAttributeElement(writer, "name", pinName);
         writeAttributeElement(writer, "group", group.c_str());
         writeAttributeElement(writer, "direction", (*i).getDirection().c_str());
         writeAttributeElement(writer, "io_standard", "LVCMOS33");

         if (pin != NULL) {
           writeAttributeElement(writer, "loc", pin->getLoc().c_str());
         }
         writeAttributeElement(writer, "user_constraints", (*i).getConstraints().c_str());

         writer.writeEndElement();
    }
}

void OutputGenerator::addTimestampPinConnections(PeripheralPort* timestampPinPort, int id) {
    map<string, CObject*> objects = dataLogger->getObjectsMap();
    for (map<string, CObject*>::iterator i = objects.begin(); i != objects.end(); i++) {
        map<string, CParameter*> timestampPins = i->second->getTimestampPins();
        for (map<string, CParameter*>::iterator itp = timestampPins.begin(); itp != timestampPins.end(); itp++) {
            i->second->getInitMethod()->getParameter(itp->first)->setValue(to_string(usedTimestampSources[id]+usedTimestampPinSources[id]+1));
            timestampInvertMask |= (i->second->getTimestampPinInvert(itp->first)) << (usedTimestampSources[id]+usedTimestampPinSources[id]);

            QString destination = itp->second->getValue().c_str();
            destination.replace(":", "_");
            usedPins.push_back(FpgaPin(destination.toStdString(), "INPUT", "PULLUP"));
            destination = "#PIN." + destination;

            CParameter* c = new CParameter("pin_source_" + to_string(usedTimestampPinSources[id]++), DataTypePin::getPinType(), false, destination.toStdString());
            timestampPinPort->newWidth(to_string(usedTimestampPinSources[id]));
            timestampPinPort->setLine(c);
        }
    }
}

void OutputGenerator::definePeripheralForSimulation(std::string name, DataType* dataType) {
    if (dataType->getName().compare("sdcard_regs_t") == 0) {
        pcPeripheralsStream << "#define " << name << " " << "\"/dev/sde\"" << endl;
    } else if (dataType->getName().compare("compare_regs_t") == 0) {
        pcPeripheralsStream << "#define " << name << " " << pcPeripheralCompareCounter++ << endl;
    } else if (dataType->getName().compare("timer_regs_t") == 0) {
        pcPeripheralsStream << "#define " << name << " " << pcPeripheralTimerCounter++ << endl;
    } else {
        pcPeripheralsStream << "#define " << name << " " << pcPeripheralIdCounter++ << endl;
    }
}

/*
 * code adapted from:
 * http://stackoverflow.com/questions/11876290/c-fastest-way-to-read-only-last-line-of-text-file
 */
std::string OutputGenerator::readLastLine(std::string fileName) {
    ifstream file;
    cerr << " !!!!!!!!!!!!!!!!!!!! " << endl << fileName << endl;
    file.open(fileName);

    if (file.is_open()) {
        file.seekg(0,std::ios_base::end);
        char ch = ' ';
        while(ch != '\n') {
            file.seekg(-2,std::ios_base::cur);
            if((int)file.tellg() <= 0){
                file.seekg(0);
                break;
            }
            file.get(ch);
        }
    }
    string res;
    getline(file, res);
    file.close();
    return res;
}

int OutputGenerator::determineMinBlockRAMcount(int cpuID) {
    string contraintFileFolder = "/build/firmware/subsystem_" + to_string(cpuID);
    string line = readLastLine(directory + contraintFileFolder + "/spartanmc_0.ucf");

    int start = line.find('[')+1;
    int end = line.find(']');
    line = line.substr(start, end-start);

    cerr << " !!!!!!!!!!!!!!!!!!!! " << endl << line << endl;

    return atoi(line.c_str()) + 1; //TODO this
    //TODO write error of could not determined
}

void OutputGenerator::flash() {
    timingError = false;
    exec("./flash.sh");
}
