#ifndef CPARAMETER_H
#define CPARAMETER_H

#include <QObject>
#include <string>
#include <map>
#include <QXmlStreamWriter>

class DataType;

class CParameter : public QObject {
    Q_OBJECT

public:
    CParameter(std::string name, DataType* dataType, bool pointer = false);
    CParameter(std::string name, DataType* dataType, bool pointer, std::string value);
    CParameter();
    CParameter(QXmlStreamReader& in);
    CParameter(const CParameter& other);

    bool sameSignature(CParameter& compare);
    std::string getName() { return name; }
    DataType* getDataType() { return dataType; }

    void setHideFromUser(bool hide) { hideFromUser = hide; }
    bool getHideFromUser() { return hideFromUser; }

    void setCritical(bool critical) { this->critical = critical; }
    bool getCritical() { return critical; }

    bool isPointer() { return pointer; }

    void setValue(std::string value) { this->value = value; emit valueChanged(value); }
    void setName(std::string name) { this->name = name; }
    std::string getValue() { return value; }
    std::string getCValue();

    void setDescription(std::string description) { this->description = description; }
    std::string getDescription() { return description; }

    friend QXmlStreamWriter& operator<<(QXmlStreamWriter& out, CParameter& parameter);
    friend QXmlStreamReader& operator>>(QXmlStreamReader& in, CParameter& parameter);
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
    void valueChanged(std::string value);
};

#endif // CPARAMETER_H
