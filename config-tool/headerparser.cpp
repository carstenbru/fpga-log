/**
 * @file headerparser.cpp
 * @brief HeaderParser class
 *
 * @author Carsten Bruns (carst.bruns@gmx.de)
 */

#include "headerparser.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <QDirIterator>

using namespace std;

std::map<std::string, std::string> HeaderParser::defines;

HeaderParser::HeaderParser(bool globalFiles) :
    globalFiles(globalFiles)
{
}

void HeaderParser::addFolder(std::string path, bool includeSubdirs) {
    QDirIterator::IteratorFlags iteratorFlags = includeSubdirs ? QDirIterator::Subdirectories : QDirIterator::NoIteratorFlags;
    QDirIterator dirIter(QString(path.c_str()), iteratorFlags);
    while (dirIter.hasNext()) {
        dirIter.next();
        if (QFileInfo(dirIter.filePath()).isFile()) {
            if (QFileInfo(dirIter.filePath()).suffix() == "h") {
                files.push_back(dirIter.filePath().toStdString());
            }
        }
    }
}

void HeaderParser::parseFiles() {
    map<DataTypeStruct*, string> inheritanceMap;
    for (list<string>::iterator i = files.begin(); i != files.end(); i++) {
        parseFileForDataTypes(*i, inheritanceMap);
    }

    for (map<DataTypeStruct*, string>::iterator i = inheritanceMap.begin(); i != inheritanceMap.end(); i++) {
        DataTypeStruct* parent = DataTypeStruct::getType(i->second);
        i->first->setSuperType(parent);
        parent->addChild(i->first);
    }

    for (list<string>::iterator i = files.begin(); i != files.end(); i++) {
        parseFileForMethods(*i);
    }
}

void HeaderParser::parseFileForDataTypes(std::string filename, std::map<DataTypeStruct*, std::string>& inheritanceMap) {
    ifstream header(filename.c_str());
    string headerDescription = "";
    bool firstDescription = true;

    if (header.is_open()) {
        istreambuf_iterator<char> headerIter(header);
        istreambuf_iterator<char> headerEnd;

        boost::char_separator<char> s(" \t\n");
        boost::tokenizer<boost::char_separator<char>, istreambuf_iterator<char> > tokens(headerIter, headerEnd, s);
        for (boost::tokenizer<boost::char_separator<char>, istreambuf_iterator<char> >::iterator i = tokens.begin(); i != tokens.end(); i++) {
            if ((*i).compare("typedef") == 0) {
                i++;
                if ((*i).compare("volatile") == 0)
                    i++;
                if ((*i).compare("struct") == 0) {
                    string superType;
                    bool hasSuper = false;
                    while ((*i).compare("}") != 0) {
                        if (!hasSuper) {
                            if ((*i).compare("super;") == 0)
                                hasSuper = true;
                            else
                                superType = *i;
                        }

                        if (++i == tokens.end())
                            return;
                    }
                    string name = *++i;
                    name.erase(name.find(';'), name.length());
                    DataTypeStruct* d = new DataTypeStruct(name, filename, globalFiles, headerDescription);
                    if (hasSuper)
                        inheritanceMap[d] = superType;
                } else if ((*i).compare("enum") == 0) {
                    i++;
                    i++;
                    list<enumVal> values;
                    stringstream commentText;
                    enumVal eVal;
                    bool firstVal = true;
                    bool comment = false;
                    bool valRefToken = false;
                    bool intToken = false;
                    while ((*i).compare("}") != 0) {
                        if (!comment) {
                          if (((*i).substr(0,2).compare("/*") == 0) || ((*i).compare("=") == 0)) {
                              commentText.str("");
                              comment = true;
                          } else {
                              if (!firstVal) {
                                  values.push_back(eVal); //add PREVIUOUS value
                                  eVal.valueReference = "";
                              }
                              firstVal = false;

                              string value = *i;
                              if (value.find(',') < value.length())
                                  value.erase(value.find(','), value.length());
                              eVal.value = value;
                          }
                        } else {
                            if (((*i).compare("*/") == 0) || ((*i).at((*i).length()-1) == ',')) {
                                eVal.description = commentText.str();
                                comment = false;
                            }
                            else if ((*i).substr(0,2).compare("/*") == 0) {
                                commentText.str("");
                                valRefToken = false;
                            } else {
                                if (i->compare("@value_ref") == 0) {
                                    valRefToken = true;
                                } else {
                                    if (!valRefToken) {
                                      commentText << *i << " ";
                                    } else {
                                        eVal.valueReference = *i;
                                        valRefToken = false;
                                    }
                                }
                            }
                        }
                        if ((*i).at((*i).length()-1) == '=') {
                            intToken = true;
                        } else if (intToken) {
                            intToken = false;
                            int intVal;
                            string s = *i;
                            if (s.find('b') != string::npos) {
                                intVal = strtol(s.c_str()+s.find('b')+1, NULL, 2);
                            } else if (s.find('x') != string::npos) {
                                intVal = strtol(s.c_str()+s.find('x')+1, NULL, 16);
                            } else {
                                istringstream(s) >> intVal;
                            }
                            eVal.intVal = intVal;
                        }
                        if (++i == tokens.end())
                            return;
                    }
                    values.push_back(eVal); //add the last value

                    string name = *++i;
                    name.erase(name.find(';'), name.length());
                    DataTypeEnumeration* d = new DataTypeEnumeration(name, filename, globalFiles);
                    d->addValues(values);
                }
            } else if ((*i).compare("#define") == 0) {
                string name = *(++i);
                defines[name] = *(++i);
            } else if ((*i).compare("@brief") == 0) {
                stringstream s;
                i++;
                while ((i != tokens.end()) && ((*i).compare("*") != 0) && ((*i).compare("*/") != 0)) {
                    s << *i << " ";
                    i++;
                }
                if (firstDescription) {
                    headerDescription = s.str();
                    firstDescription = false;
                }
            }
        }
    }
}

void HeaderParser::parseFileForMethods(string filename) {
    ifstream header(filename.c_str());

    if (header.is_open()) {
        istreambuf_iterator<char> headerIter(header);
        istreambuf_iterator<char> headerEnd;

        boost::char_separator<char> s(" \t\n");
        boost::tokenizer<boost::char_separator<char>, istreambuf_iterator<char> > tokens(headerIter, headerEnd, s);
        string returnType;
        stringstream description;
        map<string, string> paramDescMap;
        description.str("");
        for (boost::tokenizer<boost::char_separator<char>, istreambuf_iterator<char> >::iterator i = tokens.begin(); i != tokens.end(); i++) {
            string s = *i;
            size_t op = s.find_first_of('(');
            if ((op != string::npos) && (op != 0)) {
                string method_name = s;
                method_name.erase(op, s.length());

                string type;
                if (op < s.length() - 1) {
                    type = s;
                    type.erase(0, op + 1);
                }
                else {
                    type = *(++i);
                }

                string parameters = type + " ";
                i++;
                while (parameters.rfind(");") ==  string::npos) {
                    parameters.append(*i).append(" ");

                    if (++i == tokens.end())
                        return;
                }
                parameters.erase(parameters.rfind(')'), parameters.length());

                bool wasMethod = false;
                if (type.back() == '*') {
                    type.erase(type.length() - 1, 1);
                    try {
                        DataTypeStruct* dt = DataTypeStruct::getType(type);
                        string fullName = method_name;
                        if (method_name.find(dt->getCleanedName()) == 0) {
                            method_name.erase(0, dt->getCleanedName().length() + 1);

                            bool pointer = false;
                            if (returnType.back() == '*') {
                                pointer = true;
                                returnType.erase(returnType.length() - 1, 1);
                            }

                            try {
                                DataType* rt = DataType::getType(returnType);
                                CMethod* method = new CMethod(fullName, method_name, CParameter("return", rt, pointer), filename, description.str()); //TODO
                                bool inherit = true;
                                if (method_name.compare("init") == 0) {
                                    method->setHideFromUser(true);
                                    inherit = false;
                                }

                                try {
                                    wasMethod = true;
                                    parseMethodParameters(method, parameters, paramDescMap);
                                    dt->addMethod(method, inherit);
                                } catch (exception e) {
                                }
                            } catch (exception) {
                                cerr << QObject::tr("unknown type:").toStdString() << " " << returnType << endl;
                            }
                        }
                    } catch (exception) {
                        cerr << QObject::tr("unknown type:").toStdString() << " " << type << endl;
                    }
                }
                if (!wasMethod) {
                    DataTypeFunction::addFunction(method_name, returnType + ":" + parameters, globalFiles);
                }
            } else if ((*i).compare("@brief") == 0) {
                i++;
                description.str("");
                while ((i != tokens.end()) && ((*i).compare("*") != 0) && ((*i).compare("*/") != 0)) {
                    description << *i << " ";
                    i++;
                }
                paramDescMap.clear();
            } else if ((*i).compare("@param") == 0) {
                i++;
                string name = *i++;
                stringstream p;
                p.str("");
                while ((i != tokens.end()) && ((*i).compare("*") != 0) && ((*i).compare("*/") != 0)) {
                    p << *i << " ";
                    i++;
                }
                paramDescMap[name] = p.str();
            }
            returnType = *i;
        }
    }
}

void HeaderParser::parseMethodParameter(CMethod* method, std::string parameter, map<string, string>& paramDescMap) {
    boost::char_separator<char> s(" ");
    boost::tokenizer<boost::char_separator<char> > tokens(parameter, s);
    boost::tokenizer<boost::char_separator<char> >::iterator i = tokens.begin();
    while ((*i).compare("const") == 0) {
        i++;
    }
    string type = *i++;
    while ((*i).compare("const") == 0) {
        i++;
    }
    if (type.compare("unsigned") == 0) {
        type = type.append(" ").append(*i++);
    }

    string name = *i;
    bool pointer = false;
    if (type.back() == '*') {
        pointer = true;
        type.erase(type.length() - 1, 1);
    }

    if (pointer && (type.compare("char") == 0)) {
        type = "string";
    }

    try {
        CParameter param(name, DataType::getType(type), pointer);
        if (type.compare("parameter_type_t") == 0) {
            param.setCritical(true);
        }
        try {
            param.setDescription(paramDescMap.at(name));
        } catch (exception) {
        }
        method->addParameter(param);
    } catch (exception e) {
        cerr << QObject::tr("method parameter parsing error: unknown type:").toStdString() << " " << type << endl;
        throw e;
    }
}

void HeaderParser::parseMethodParameters(CMethod* method, std::string parameters, map<string, string>& paramDescMap) {
    boost::char_separator<char> s(",");
    boost::tokenizer<boost::char_separator<char> > tokens(parameters, s);
    for (boost::tokenizer<boost::char_separator<char> >::iterator i = tokens.begin(); i != tokens.end(); i++) {
        string parameter = *i;
        parameter.erase(0, parameter.find_first_not_of(" "));
        size_t op = parameter.find("(");
        if (op != string::npos) {
            string signature = parameter;
            signature.erase(op-1, signature.length());

            string name = parameter;
            name.erase(0, op+2);
            name.erase(name.find(")"), name.length());

            parameter.erase(0, parameter.find(")")+2);

            while (parameter.back() != ')') {
                if (++i == tokens.end()) {
                    return;
                }
                parameter += "," + *i;
            }
            parameter.erase(parameter.length() - 1, parameter.length());
            signature += ":" + parameter;
            method->addParameter(CParameter(name, DataTypeFunction::getType(signature), true));
        } else {
            parseMethodParameter(method, parameter, paramDescMap);
        }
    }
}

std::string HeaderParser::getDefinedString(std::string name) {
    try {
        return defines.at(name);
    } catch (exception) {
        cerr << QObject::tr("symbol %1 is not defined").arg(name.c_str()).toStdString() << endl;
        return "";
    }
}

int HeaderParser::getDefinedInteger(std::string name) {
    try {
        return atoi(getDefinedString(name).c_str());
    } catch (exception) {
        cerr << QObject::tr("symbol %1 is not a number").arg(name.c_str()).toStdString() << endl;
        return 0;
    }
}
