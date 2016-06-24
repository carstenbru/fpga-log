/**
 * @file cparameter.h
 * @brief CParameter class
 *
 * @author Carsten Bruns (carst.bruns@gmx.de)
 */

#ifndef CPARAMETER_H
#define CPARAMETER_H

#include <QObject>
#include <string>
#include <map>
#include <QXmlStreamWriter>

class DataType;

/**
 * @brief class representing a parameter
 *
 * This class models a parameter with name, type and value. It used for e.g.:
 * - a module parameter
 * - pin assignments
 * - a parameter to a function call of a module
 * - datalogger paramters like the target platform
 * - ...
 */
class CParameter : public QObject {
    Q_OBJECT

public:
    /**
     * @brief generates a parameter with the default value
     *
     * @param name name of the parameter
     * @param dataType data type of the parameter
     * @param pointer true if the parameter is a pointer and not an immediate value
     */
    CParameter(std::string name, DataType* dataType, bool pointer = false);

    /**
     * @brief generates a parameter with an assigned value
     *
     * @param name name of the parameter
     * @param dataType data type of the parameter
     * @param pointer true if the parameter is a pointer and not an immediate value
     * @param value value of the parameter
     */
    CParameter(std::string name, DataType* dataType, bool pointer, std::string value);

    /**
     * @brief generates an empty parameter
     */
    CParameter();

    /**
     * @brief generates a parameter from a XML description
     *
     * @param in XML reader with the description
     */
    CParameter(QXmlStreamReader& in);

    /**
     * @brief copy constructor
     *
     * @param other source parameter
     */
    CParameter(const CParameter& other);

    /**
     * @brief checks if the signature (i.e. same type, pointer) of this parameter is equal to the one of another parameter
     *
     * @param compare other parameter
     * @return true if both paramters have the same signature, otherwise false
     */
    bool sameSignature(CParameter& compare);

    /**
     * @brief gets the name of a parameter
     *
     * @return the name of a parameter
     */
    std::string getName() { return name; }

    /**
     * @brief gets the data type of the parameter
     *
     * @return the data type of the parameter
     */
    DataType* getDataType() { return dataType; }

    /**
     * @brief sets if the parameter should be hidden from the user (i.e. not shown in configuration dialoges)
     *
     * @param hide true to hide it, false to show it
     */
    void setHideFromUser(bool hide) { hideFromUser = hide; }

    /**
     * @brief gets if the parameter should be hidden from the user (i.e. not shown in configuration dialoges)
     * @return true if the parameter should be hidden from the user, false if it should be shown
     */
    bool getHideFromUser() { return hideFromUser; }

    /**
     * @brief sets if the parameter is critical (i.e. referenced by something else)
     *
     * @param critical true if the parameter is critical
     */
    void setCritical(bool critical) { this->critical = critical; }

    /**
     * @brief gets if the parameter is critical (i.e. referenced by something else)
     * @return true if the parameter is critical
     */
    bool getCritical() { return critical; }

    /**
     * @brief checks if the parameter is a pointer
     *
     * @return true if the parameter is a pointer, otherwise false
     */
    bool isPointer() { return pointer; }

    /**
     * @brief updates the value of the parameter
     *
     * @param value new parameter value
     */
    void setValue(std::string value) { this->value = value; emit valueChanged(value); }

    /**
     * @brief updates the name of the parameter
     * @param name new name of the parameter
     */
    void setName(std::string name) { this->name = name; }

    /**
     * @brief gets the parameters value
     * @return the parameters value
     */
    std::string getValue() { return value; }

    /**
     * @brief gets the parameter value as it can be used in c-code (e.g. adding quotation marks if it is a string)
     *
     * @return the parameter value as it can be used in c-code
     */
    std::string getCValue();

    /**
     * @brief sets the descriotion (help text) of the parameter
     * @param description new description
     */
    void setDescription(std::string description) { this->description = description; }

    /**
     * @brief gets the descriotion (help text) of the parameter
     * @return the descriotion (help text) of the parameter
     */
    std::string getDescription() { return description; }

    /**
     * @brief writes a XML representation
     * @param out XML stream writer
     * @param parameter parameter to write
     * @return reference to the XML writer (to chain the operator)
     */
    friend QXmlStreamWriter& operator<<(QXmlStreamWriter& out, CParameter& parameter);

    /**
     * @brief reads a XML representation
     * @param in XML stream reader
     * @param parameter destination parameter
     * @return reference to the XML reader (to chain the operator)
     */
    friend QXmlStreamReader& operator>>(QXmlStreamReader& in, CParameter& parameter);

    /**
     * @brief loads a parameter from a XML stream
     *
     * @param in XML stream reader
     */
    void loadFromXmlStream(QXmlStreamReader& in);
private:
    std::string name;
    std::string value;
    std::string description;

    bool hideFromUser;
    bool critical;

    DataType* dataType;
    bool pointer;
signals:
    /**
     * @brief signal triggered when the parameters value changed
     *
     * @param value the new value of the parameter
     */
    void valueChanged(std::string value);
};

#endif // CPARAMETER_H
