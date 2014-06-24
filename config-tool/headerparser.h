#ifndef HEADERPARSER_H
#define HEADERPARSER_H

#include <map>
#include <list>
#include <string>
#include "datatype.h"

class HeaderParser
{
public:
    HeaderParser();

    void addFolder(std::string path, bool includeSubdirs, bool systemDir);
    void parseFiles();
private:
    void parseFileForDataTypes(std::string filename, std::map<DataType *, std::string> &inheritanceList, bool systemFile);
    void parseFileForMethods(std::string filename);

    void parseMethodParameters(CMethod* method, std::string parameters);
    void parseMethodParameter(CMethod* method, std::string parameter);

    std::list<std::string> files;
    std::list<bool> filesIsSystem;
};

#endif // HEADERPARSER_H
