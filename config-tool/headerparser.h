/**
 * @file headerparser.h
 * @brief HeaderParser class
 *
 * @author Carsten Bruns (carst.bruns@gmx.de)
 */

#ifndef HEADERPARSER_H
#define HEADERPARSER_H

#include <map>
#include <list>
#include <string>
#include <boost/tokenizer.hpp>
#include "datatype.h"

/**
 * @brief class for parsing of fpga-log header files (e.g. for module and method definitions)
 */
class HeaderParser
{
public:
    /**
     * @brief constructor
     *
     * @param globalFiles true if the files parsed with this instance are global headers of fpga-log, false if they are custom user header for a specific system
     */
    HeaderParser(bool globalFiles);

    /**
     * @brief adds a folder with all files in it to the parser
     *
     * @param path the path of the folder
     * @param includeSubdirs true to also search in sub-directories
     */
    void addFolder(std::string path, bool includeSubdirs);

    /**
     * @brief starts the actual parsing
     */
    void parseFiles();

    /**
     * @brief resolves a c-define (#define) declared in the parsed header files
     *
     * @param name key to look up
     * @return value the key is defined to
     */
    static std::string getDefinedString(std::string name);

    /**
     * @brief resolves a integer c-define (#define) declared in the parsed header files
     *
     * @param name key to look up
     * @return value the key is defined to
     */
    static int getDefinedInteger(std::string name);
private:
    void parseFileForDataTypes(std::string filename, std::map<DataTypeStruct *, std::string> &inheritanceList);
    void parseFileForMethods(std::string filename);

    void parseMethodParameters(CMethod* method, std::string parameters, std::map<std::string, std::string> &paramDescMap);
    void parseMethodParameter(CMethod* method, std::string parameter, std::map<std::string, std::string> &paramDescMap);

    bool globalFiles;
    std::list<std::string> files;

    static std::map<std::string, std::string> defines;
};

#endif // HEADERPARSER_H
