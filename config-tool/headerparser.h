#ifndef HEADERPARSER_H
#define HEADERPARSER_H

#include <map>
#include <list>
#include <string>
#include <boost/tokenizer.hpp>
#include "datatype.h"

class HeaderParser
{
public:
    HeaderParser(bool globalFiles);

    void addFolder(std::string path, bool includeSubdirs);
    void parseFiles();

    static std::string getDefinedString(std::string name);
    static int getDefinedInteger(std::string name);
private:
    void parseFileForDataTypes(std::string filename, std::map<DataTypeStruct *, std::string> &inheritanceList);
    void parseFileForMethods(std::string filename);

    void parseMethodParameters(CMethod* method, std::string parameters);
    void parseMethodParameter(CMethod* method, std::string parameter);

    bool globalFiles;
    std::list<std::string> files;

    static std::map<std::string, std::string> defines;
};

#endif // HEADERPARSER_H
