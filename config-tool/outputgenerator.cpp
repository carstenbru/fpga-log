#include "outputgenerator.h"
#include <list>
#include <string>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <QCoreApplication>
#include <QDirIterator>
#include "cobject.h"

using namespace std;

OutputGenerator::OutputGenerator(DataLogger *dataLogger, string directory) :
    dataLogger(dataLogger),
    directory(directory),
    usedIdCounter(0),
    usedTimestampSources(0),
    usedTimestampPinSources(0),
    process(this),
    busy(false),
    error(false),
    pcPeripheralIdCounter(0),
    pcPeripheralCompareCounter(0),
    pcPeripheralTimerCounter(0),
    timestampInvertMask(0)
{
    cout << directory << endl;
    process.setWorkingDirectory(directory.c_str());
    connect(&process, SIGNAL(readyReadStandardOutput()), this, SLOT(newChildStdOut()));
    connect(&process, SIGNAL(readyReadStandardError()), this, SLOT(newChildErrOut()));
    connect(&process, SIGNAL(finished(int)), this, SLOT(processFinished()));

    copyProjectTemplate();
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

            QDir().mkpath(QFileInfo(newPath).dir().absolutePath());
            if (dirIter.fileName().compare("main.c") == 0) {
                copyMainC(dirIter.filePath(), newPath);
            } else {
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
        emit finished(error || !synthesisSuccessful || timingError, timingError);
    } else {
        process.start(QString(pending.front().c_str()));
        pending.pop_front();
    }
}

void OutputGenerator::checkSynthesisMessage(string message) {
    if (message.find("System synthesis complete.") != string::npos) {
        synthesisSuccessful = true;
    }
    if (message.find("Timing: Completed") != string::npos) {
        if (message.find("Timing: Completed - No errors found.") == string::npos) {
            timingError = true;
        }
    }
}

void OutputGenerator::newChildStdOut() {
    string s = QString(process.readAllStandardOutput()).toStdString();
    cout << s;
    checkSynthesisMessage(s);
}

void OutputGenerator::newChildErrOut() {
    cerr << QString(process.readAllStandardError()).toStdString();
}

void OutputGenerator::generateConfigFiles() {
    generateSystemXML();
    generateCSource();;

    exec("make jconfig +args=\"--generate system.xml\"");
}

void OutputGenerator::synthesizeSystem() {
    generateConfigFiles();

    timingError = false;
    synthesisSuccessful = false;
    exec("make all");
}

void OutputGenerator::generateCSource() {
    ofstream file;
    file.open(directory + "/firmware/src/logger_config.c");

    ostream& stream = file;

    writePreamble(stream);
    stream << endl;

    stringstream tmpFile;

    writeVariableDefinitions(tmpFile);
    tmpFile << endl;
    writeInitFunction(tmpFile);
    tmpFile << endl;
    writeConnectPorts(tmpFile);
    tmpFile << endl;
    writeAdvancedConfig(tmpFile);

    writeHeaderIncludes(stream);
    stream << endl << tmpFile.str();

    file.close();

    file.open(directory + "/firmware/include/pc_peripherals.h");
    writePreamble(file);
    file << endl << "#ifndef PERIPHERALS_SIM_H_" << endl << "#define PERIPHERALS_SIM_H_" << endl << endl;
    file << "#define PIPE_COUNT " << pcPeripheralIdCounter << endl << endl;
    file << "#define TIMER_COUNT " << pcPeripheralTimerCounter << endl;
    file << "#define COMPARE_COUNT " << pcPeripheralCompareCounter << endl << endl;
    file << pcPeripheralsStream.str() << endl;
    file << "#define TIMESTAMP_CAPTURE_SIGNALS_COUNT " << usedTimestampSources << endl;
    string tscs = pcTimestampCaptureStream.str();
    tscs.erase(0, 2);
    file << "#define TIMESTAMP_CAPTURE_SIGNALS {" << tscs << "}" << endl;
    file << endl << "#endif" << endl;
    file.close();

    cout << "C-Konfigurationsdatei erfolgreich geschrieben." << endl;
}

void OutputGenerator::writeVariableDefinitions(std::ostream& stream) {
    map<string, CObject*> objects = dataLogger->getObjectsMap();

    for (map<string, CObject *>::iterator i = objects.begin(); i != objects.end(); i++) {
        putUsedHeader(i->second->getType()->getHeaderName(), i->second->getType()->isGlobal());
        stream << i->second->getType()->getName() << "\t" << i->first << ";" << endl;
    }
}

void OutputGenerator::writeInitFunction(ostream& stream) {
    map<string, CObject*> objects = dataLogger->getObjectsMap();

    stream << "void init_objects(void) {" << endl;

    map<string, bool> initDone;
    for (map<string, CObject *>::iterator i = objects.begin(); i != objects.end(); i++) {
        writeObjectInit(stream, i->second, objects, initDone);
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
        putUsedHeader(init->getHeaderName(), object->getType()->isGlobal());
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
                    writeObjectInit(stream, paramObject, objects, initDone);
                }
            } catch (exception) {
                if ((*i).getDataType()->hasSuffix("_regs_t")) {
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
                cerr << "FEHLER: Parameter " << (*i).getName() << " von Objekt " << object->getName() << " nicht gesetzt!" << endl;
            }

            tmpStream << value;
        }
        stream << tmpStream.str() << ");" << endl;
        initDone[object->getName()] = true;
    }
}

void OutputGenerator::writeConnectPorts(ostream& stream) {
    stream << "void connect_ports(void) {" << endl;

    list<DatastreamObject*> modules = dataLogger->getDatastreamModules();
    for (list<DatastreamObject*>::iterator i = modules.begin(); i != modules.end(); i++) {
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
    putUsedHeader(method->getHeaderName(), object->getType()->isGlobal());
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
            cerr << "FEHLER: Parameter " << (*i).getName() << " von Objekt " << object->getName() << " nicht gesetzt!" << endl;
        }

        stream << value;
    }
    stream << ");" << endl;
}

void OutputGenerator::writeAdvancedConfig(std::ostream& stream) {
    stream << "void advanced_config(void) {" << endl;

    map<string, CObject*> objects = dataLogger->getObjectsMap();

    for (map<string, CObject *>::iterator i = objects.begin(); i != objects.end(); i++) {
        CObject* object = i->second;
        list<CMethod*> methods = object->getAdvancedConfig();
        for (list<CMethod*>::iterator mIt = methods.begin(); mIt != methods.end(); mIt++) {
            writeMethod(stream, object, *mIt, objects);
        }
    }

    stream << "}" << endl;
}

void OutputGenerator::generateSystemXML() {
    ifstream templateFile("../config-tool-files/template.xml");
    if (!templateFile.is_open()) {
        cerr << "System Template XML konnte nicht geöffnet werden." << endl;
        return;
    }

    ofstream file;
    file.open(directory + "/system.xml");
    ostream& stream = file;

    QString targetNode;
    QXmlStreamWriter targetWriter(&targetNode);
    targetWriter.setAutoFormatting(true);
    writeTargetNode(targetWriter);

    QString peripherals;
    QXmlStreamWriter peripheralsWriter(&peripherals);
    peripheralsWriter.setAutoFormatting(true);
    writePeripherals(peripheralsWriter);

    QString timestampConnections;
    QXmlStreamWriter timestampConnectionsWriter(&timestampConnections);
    timestampConnectionsWriter.setAutoFormatting(true);
    usedTimestampSources = max(1, usedTimestampSources);
    writeTimestampPins(timestampConnectionsWriter);

    QString pins;
    QXmlStreamWriter pinsWriter(&pins);
    pinsWriter.setAutoFormatting(true);
    writePins(pinsWriter);

    while (!templateFile.eof()) {
        string line;
        getline(templateFile, line);
        if (line.compare("TARGET_NODE") == 0) {
            stream << targetNode.toStdString() << endl;
        } else if (line.compare("FPGA-LOG_PERIPHERALS") == 0) {
            stream << peripherals.toStdString() << endl;
        } else if (line.compare("PERI_CLOCK_ATTRIBUTE") == 0) {
            stream << "<attribute id=\"value\">" << dataLogger->getPeriClk() << "</attribute>" << endl;
        } else if (line.compare("CLOCK_PERIOD_ATTRIBUTE") == 0) {
            stream << "<attribute id=\"value\">" << (1000000000.0f / dataLogger->getClk()) << "</attribute>" << endl;
        } else if (line.compare("SYSTEM_CLOCK_PERIOD_ATTRIBUTE") == 0) {
            stream << "<attribute id=\"value\">" << (1000000000.0f / dataLogger->getSystemClk()) << "</attribute>" << endl;
        } else if (line.compare("CLOCK_DIVIDE_ATTRIBUTE") == 0) {
            stream << "<attribute id=\"value\">" << dataLogger->getClkDivide() << "</attribute>" << endl;
        } else if (line.compare("CLOCK_MULTIPLY_ATTRIBUTE") == 0) {
            stream << "<attribute id=\"value\">" << dataLogger->getClkMultiply() << "</attribute>" << endl;
        } else if (line.compare("TIMESTAMP_GEN_SOURCES_ATTRIBUTE") == 0) {
            stream << "<attribute id=\"value\">" << usedTimestampSources << "</attribute>" << endl;
        } else if (line.compare("TIMESTAMP_GEN_PIN_SOURCES_ATTRIBUTE") == 0) {
            stream << "<attribute id=\"value\">" << max(1, usedTimestampPinSources) << "</attribute>" << endl;
        } else if (line.compare("TIMESTAMP_GEN_INVERTED_SOURCES_MASK_ATTRIBUTE") == 0) {
            stream << "<attribute id=\"value\">" << timestampInvertMask << "</attribute>" << endl;
        } else if (line.compare("FPGA_PINS") == 0) {
            stream << pins.toStdString() << endl;
        } else if (line.compare("TIMESTAMP_GEN_PIN_CONNECTIONS") == 0) {
            stream << timestampConnections.toStdString() << endl;
        } else if (line.compare("2000HZ_DIV_VALUE_ATTRIBUTE") == 0) {
            stream << "<attribute id=\"value\">" << (dataLogger->getPeriClk() / 2000) << "</attribute>" << endl;
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

void OutputGenerator::writePeripheral(QXmlStreamWriter& writer, SpmcPeripheral* peripheral) {
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
                        destName = "SUBSYSTEM/" + peripheral->getParentName() + "_" + destName;
                        transform(destName.begin(), destName.end(), destName.begin(), ::toupper);
                        destination.remove(0, slash);
                        destination = destName.c_str() + destination;
                    } else if (destination.compare("TIMESTAMP_GEN") == 0) { //timestamp generator source
                        destination = "SUBSYSTEM/TIMESTAMP_GEN/#PORT.internal_source";
                        string param = (*portIt)->getName();
                        peripheral->getParentObject()->getInitMethod()->getParameter(param)->setValue(to_string(++usedTimestampSources));
                        pcTimestampCaptureStream << ", " << peripheralNameUpper;
                    }
                    writeConnection(writer, destination.toStdString(), lsb++);
                }
            }

            writer.writeEndElement();
        }
    }

    writeSpmcConnections(writer);

    writer.writeEndElement();
}

void OutputGenerator::writePeripherals(QXmlStreamWriter& writer) {
    map<string, CObject*> objects = dataLogger->getObjectsMap();
    for (map<string, CObject*>::iterator i = objects.begin(); i != objects.end(); i++) {
        list<SpmcPeripheral*> peripherals = i->second->getPeripherals();
        for (list<SpmcPeripheral*>::iterator ip = peripherals.begin(); ip != peripherals.end(); ip++) {
            writePeripheral(writer, *ip);
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

void OutputGenerator::writeSpmcConnections(QXmlStreamWriter& writer) {
    writePortConnection(writer, "access_peri", "SUBSYSTEM/SPARTANMC/#PORT.access_peri", 0);
    writePortConnection(writer, "addr_peri", "SUBSYSTEM/SPARTANMC/#PORT.addr_peri", 0);
    writePortConnection(writer, "clk_peri", "SUBSYSTEM/SPARTANMC/#PORT.clk_peri", 0);
    writePortConnection(writer, "do_peri", "SUBSYSTEM/SPARTANMC/#PORT.do_peri", 0);
    writePortConnection(writer, "reset", "SUBSYSTEM/SPARTANMC/#PORT.reset", 0);
    writePortConnection(writer, "wr_peri", "SUBSYSTEM/SPARTANMC/#PORT.wr_peri", 0);
    writePortConnection(writer, "di_peri", "SUBSYSTEM/SPARTANMC/#PORT.di_peri", 0);
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

void OutputGenerator::writeTimestampPins(QXmlStreamWriter& writer) {
    map<string, CObject*> objects = dataLogger->getObjectsMap();
    for (map<string, CObject*>::iterator i = objects.begin(); i != objects.end(); i++) {
        map<string, CParameter*> timestampPins = i->second->getTimestampPins();
        for (map<string, CParameter*>::iterator itp = timestampPins.begin(); itp != timestampPins.end(); itp++) {
            i->second->getInitMethod()->getParameter(itp->first)->setValue(to_string(usedTimestampSources+usedTimestampPinSources+1));
            timestampInvertMask |= (i->second->getTimestampPinInvert(itp->first)) << (usedTimestampSources+usedTimestampPinSources);

            QString destination = itp->second->getValue().c_str();
            destination.replace(":", "_");
            usedPins.push_back(FpgaPin(destination.toStdString(), "INPUT", "PULLUP"));
            destination = "#PIN." + destination;

            writeConnection(writer, destination.toStdString(), usedTimestampPinSources++);  
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

void OutputGenerator::flash() {
    exec("./flash.sh");
}
