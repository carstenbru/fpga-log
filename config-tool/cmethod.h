/**
 * @file cmethod.h
 * @brief CMethod class
 *
 * @author Carsten Bruns (carst.bruns@gmx.de)
 */

#ifndef CMETHOD_H
#define CMETHOD_H

#include <string>
#include <list>
#include "cparameter.h"

//CObject forward declariation,including the header here would lead to a cycle
class CObject;

/**
 * @brief class representing a fpga-log method, i.e. a method which can be called on a datalogger object
 */
class CMethod
{
public:
    /**
     * @brief main constructor
     *
     * @param completeName complete name of the method, including object prefix, etc.
     * @param name real name of the method (e.g. "init")
     * @param returnType return value definition of the method
     * @param headerFile header file where the method was found
     * @param description description of the method (e.g. to show in GUI)
     */
    CMethod(std::string completeName, std::string name, CParameter returnType, std::string headerFile, std::string description);

    /**
     * @brief simplified constructor using empty values for unknown fields
     * @param name real name of the method (e.g. "init")
     * @param returnType return value definition of the method
     */
    CMethod(std::string name, CParameter returnType) : CMethod("", name, returnType, "", "") {}

    /**
     * @brief constructor loading a method from a XML description
     *
     * @param in XML description reader
     * @param currentSignature current signature (as other CMethod instance) of the method to check for definition updates
     * @param object object the method should belong to
     */
    CMethod(QXmlStreamReader& in, CMethod *currentSignature, CObject* object);
    ~CMethod();

    /**
     * @brief adds a parameter to a methods signature
     *
     * @param paramter the new parameter
     */
    void addParameter(CParameter paramter) { parameters.push_back(paramter); }

    /**
     * @brief checks whether or not another method has the same signature as this one
     *
     * @param compare method instance to compare the signature to
     * @return true if the methods have identical signatures
     */
    bool sameSignature(CMethod& compare);

    /**
     * @brief gets the real name of the method
     *
     * @return the name of the method
     */
    std::string getName() { return name; }

    /**
     * @brief gets the complete name of the method, including object prefix, etc.
     * @return the complete name of the method
     */
    std::string getCompleteName() { return completeName; }

    /**
     * @brief gets the parameters of the method
     *
     * @return the parameters of the method
     */
    std::list<CParameter>* getParameters() { return &parameters; }

    /**
     * @brief gets pointers to all method parameters
     * @return a list of pointers to all method parameters
     */
    std::list<CParameter*> getMethodParameterPointers();

    /**
     * @brief gets a parameter by name
     *
     * @param name name of the parameter
     * @return the requested parameter
     */
    CParameter* getParameter(std::string name);

    /**
     * @brief gets the name of the header in which the method is declared
     *
     * @return the name of the header
     */
    std::string getHeaderName() { return headerFile; }

    /**
     * @brief gets the description (e.g. to show in GUI) of the method
     *
     * @return the description of the method
     */
    std::string getDescription() { return description; }

    /**
     * @brief sets whether the method should be hidden from the user, i.e. not shown in GUI dialogs
     *
     * @param hide new hide status
     */
    void setHideFromUser(bool hide) { hideFromUser = hide; }

    /**
     * @brief gets whether the method should be hidden from the user, i.e. not shown in GUI dialogs
     *
     * @return true if the method should be hidden from the user
     */
    bool getHideFromUser() { return hideFromUser; }

    /**
     * @brief writes the method to a XML stream
     *
     * @param out XML stream destination
     * @param cMethod method to write
     * @return destination XML stream for operator concatenation
     */
    friend QXmlStreamWriter& operator<<(QXmlStreamWriter& out, CMethod& cMethod);
private:
    bool setParameter(CParameter& newValue);

    std::string completeName;

    std::string name;
    std::string headerFile;
    std::string description;

    CParameter returnType;
    std::list<CParameter> parameters;

    bool hideFromUser;
};

#endif // CMETHOD_H
