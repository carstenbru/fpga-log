#include "outputgenerator.h"
#include <list>
#include <string>
#include <algorithm>
#include <sstream>
#include "cobject.h"

using namespace std;

OutputGenerator::OutputGenerator(DataLogger *dataLogger) :
    dataLogger(dataLogger)
{
}

void OutputGenerator::generateCSource() {
    ostream& stream = cout; //TODO
    stringstream tmpFile;

    writeVariableDefinitions(tmpFile);
    tmpFile << endl;
    writeInitFunction(tmpFile);
    tmpFile << endl;
    writeConnectPorts(tmpFile);

    writeHeaderIncludes(stream);
    stream << endl << tmpFile.str();
}

void OutputGenerator::writeVariableDefinitions(std::ostream& stream) {
    map<string, CObject*> objects = dataLogger->getObjectsMap();

    for (map<string, CObject *>::iterator i = objects.begin(); i != objects.end(); i++) {
        usedHeaders.insert(i->second->getType()->getHeaderName());
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

void OutputGenerator::writeObjectInit(std::ostream& stream, CObject* object, std::map<string, CObject *>& objects, map<string, bool>& initDone) {
    if (!initDone[object->getName()]) {
        stringstream tmpStream;

        CMethod* init = object->getInitMethod();
        usedHeaders.insert(init->getHeaderName());
        list<CParameter>* params = init->getParameters();
        tmpStream << "  " << object->getType()->getCleanedName() << "_"
                  << init->getName() << "(&" << object->getName();
        for (list<CParameter>::iterator i = ++params->begin(); i != params->end(); i++) {
            tmpStream << ", ";

            string value  = (*i).getValue();
            try {
                CObject* paramObject = objects.at(value);
                tmpStream << "&";
                if (!initDone[value]) {
                    writeObjectInit(stream, paramObject, objects, initDone);
                }
            } catch (exception) {
                if ((*i).getDataType()->hasSuffix("_regs_t")) {
                    value = object->getName() + "_" + (*i).getName();
                    transform(value.begin(), value.end(), value.begin(), ::toupper);
                }
            }

            if (value.empty()) {
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
    for (set<string>::iterator i = usedHeaders.begin(); i != usedHeaders.end(); i++) {
        string s = *i;
        s.erase(0, s.find("include") + 8);
        stream << "#include <fpga-log/" << s << ">" << endl;
    }
}
