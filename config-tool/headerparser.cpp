#include "headerparser.h"
#include <iostream>
#include <fstream>
#include <QDirIterator>

#include <boost/tokenizer.hpp>

using namespace std;
using namespace boost;

std::map<std::string, std::string> HeaderParser::defines;

HeaderParser::HeaderParser()
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
    map<DataType*, string> inheritanceMap;
    for (list<string>::iterator i = files.begin(); i != files.end(); i++) {
        parseFileForDataTypes(*i, inheritanceMap);
    }

    for (map<DataType*, string>::iterator i = inheritanceMap.begin(); i != inheritanceMap.end(); i++) {
        DataType* parent = DataType::getType(i->second);
        i->first->setSuperType(parent);
        parent->addChild(i->first);
    }

    for (list<string>::iterator i = files.begin(); i != files.end(); i++) {
        parseFileForMethods(*i);
    }
}

void HeaderParser::parseFileForDataTypes(std::string filename, std::map<DataType*, std::string>& inheritanceMap) {
    ifstream header(filename.c_str());

    if (header.is_open()) {
        istreambuf_iterator<char> headerIter(header);
        istreambuf_iterator<char> headerEnd;

        char_separator<char> s(" \t\n");
        tokenizer<char_separator<char>, istreambuf_iterator<char> > tokens(headerIter, headerEnd, s);
        for (tokenizer<char_separator<char>, istreambuf_iterator<char> >::iterator i = tokens.begin(); i != tokens.end(); i++) {
            if ((*i).compare("typedef") == 0) {
                if ((*++i).compare("struct") == 0) {
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
                    DataType* d = new DataType(name);
                    if (hasSuper)
                        inheritanceMap[d] = superType;
                }
            } else if ((*i).compare("#define") == 0) {
                string name = *(++i);
                defines[name] = *(++i);
            }
        }
    }
}

void HeaderParser::parseFileForMethods(string filename) {
    ifstream header(filename.c_str());

    if (header.is_open()) {
        istreambuf_iterator<char> headerIter(header);
        istreambuf_iterator<char> headerEnd;

        char_separator<char> s(" \t\n");
        tokenizer<char_separator<char>, istreambuf_iterator<char> > tokens(headerIter, headerEnd, s);
        string returnType;
        for (tokenizer<char_separator<char>, istreambuf_iterator<char> >::iterator i = tokens.begin(); i != tokens.end(); i++) {
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
                if (type.back() == '*') {
                    type.erase(type.length() - 1, 1);
                    try {
                        DataType* dt = DataType::getType(type);
                        if (method_name.find(dt->getCleanedName()) == 0) {
                            method_name.erase(0, dt->getCleanedName().length() + 1);

                            bool pointer = false;
                            if (returnType.back() == '*') {
                                pointer = true;
                                returnType.erase(returnType.length() - 1, 1);
                            }

                            try {
                                DataType* rt = DataType::getType(returnType);
                                CMethod* method = new CMethod(method_name, new CMethodParameter("return", rt, pointer));

                                string parameters;
                                while (parameters.rfind(");") ==  string::npos) {
                                    parameters.append(*i).append(" ");

                                    if (++i == tokens.end())
                                        return;
                                }
                                parameters.erase(0, parameters.find('(') + 1);
                                parameters.erase(parameters.rfind(')'), parameters.length());

                                try {
                                    parseMethodParameters(method, parameters);
                                    dt->addMethod(method);
                                } catch (exception e) {
                                }
                            } catch (exception) {
                                cerr << "unknown type: " << returnType << endl;
                            }
                        }
                    } catch (exception) {
                        cerr << "unknown type: " << type << endl;
                    }
                }
            }
            returnType = *i;
        }
    }
}

void HeaderParser::parseMethodParameter(CMethod* method, std::string parameter) {
    char_separator<char> s(" ");
    tokenizer<char_separator<char> > tokens(parameter, s);
    tokenizer<char_separator<char> >::iterator i = tokens.begin();
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
    try {
        method->addParameter(new CMethodParameter(name, DataType::getType(type), pointer));
    } catch (exception e) {
        cerr << "method parameter parsing error: unknown type: " << type << endl;
        throw e;
    }
}

void HeaderParser::parseMethodParameters(CMethod* method, std::string parameters) {
    char_separator<char> s(",");
    tokenizer<char_separator<char> > tokens(parameters, s);
    for (tokenizer<char_separator<char> >::iterator i = tokens.begin(); i != tokens.end(); i++) {
        parseMethodParameter(method, *i);
    }
}

std::string HeaderParser::getDefinedString(std::string name) {
    try {
        return defines.at(name);
    } catch (exception) {
        cerr << "symbol " << name << " is not defined" << endl;
        return "";
    }
}

int HeaderParser::getDefinedInteger(std::string name) {
    try {
        return atoi(getDefinedString(name).c_str());
    } catch (exception) {
        cerr << "symbol " << name << " is not a number" << endl;
        return 0;
    }
}