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
    /**
     * @brief parses a file for datatypes
     *
     * @param filename file to parse
     * @param inheritanceList map of inheritances which should be established later (as the referenced type might not exist yet) (key: type, value: super data type)
     */
    void parseFileForDataTypes(std::string filename, std::map<DataTypeStruct *, std::string> &inheritanceList);
    /**
     * @brief parses a file for methods
     *
     * @param filename file to parse
     */
    void parseFileForMethods(std::string filename);

    /**
     * @brief pareses the parameter definition of a method
     *
     * @param method destination method
     * @param parameters parameters string to parse
     * @param paramDescMap map of parameter descriptions obtained from code documentation (key: paramter name, value: description)
     */
    void parseMethodParameters(CMethod* method, std::string parameters, std::map<std::string, std::string> &paramDescMap);
    /**
     * @brief parses a single parameter definition
     *
     * @param method destination method
     * @param parameter parameter string to parse
     * @param paramDescMap map of parameter descriptions obtained from code documentation (key: paramter name, value: description)
     */
    void parseMethodParameter(CMethod* method, std::string parameter, std::map<std::string, std::string> &paramDescMap);

    bool globalFiles; /**< true if the files parsed with this instance are global headers of fpga-log, false if they are custom user header for a specific system */
    std::list<std::string> files; /**< list of files to parse */

    static std::map<std::string, std::string> defines; /**< static map of all defines found in the headers (key: name, value: defined value for the key)*/
};

#endif // HEADERPARSER_H
