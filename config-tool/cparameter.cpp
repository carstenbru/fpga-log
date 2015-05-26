#include "cparameter.h"
#include "datatype.h"
#include <iostream>

CParameter::CParameter(std::string name, DataType* dataType, bool pointer) :
    QObject(),
    name(name),
    value(dataType->getDefaultValue()),
    hideFromUser(false),
    critical(false),
    dataType(dataType),
    pointer(pointer)
{
}

CParameter::CParameter(std::string name, DataType* dataType, bool pointer, std::string value) :
    name(name),
    value(value),
    hideFromUser(false),
    critical(false),
    dataType(dataType),
    pointer(pointer)
{
}

CParameter::CParameter(const CParameter& other) :
    QObject(),
    name(other.name),
    value(other.value),
    description(other.description),
    hideFromUser(other.hideFromUser),
    critical(false),
    dataType(other.dataType),
    pointer(other.pointer)
{

}

CParameter::CParameter() :
    name(""),
    value(""),
    hideFromUser(false),
    critical(false),
    dataType(NULL),
    pointer(false)
{

}

CParameter::CParameter(QXmlStreamReader& in) {
    loadFromXmlStream(in);
}

bool CParameter::sameSignature(CParameter &compare) {
    return ((dataType == compare.dataType) && (pointer == compare.pointer));
}

std::string CParameter::getCValue() {
    return dataType->getCValue(value);
}

void CParameter::loadFromXmlStream(QXmlStreamReader& in) {
    const QXmlStreamAttributes& a = in.attributes();
    name = a.value("name").toString().toStdString();
    value = a.value("value").toString().toStdString();
    hideFromUser = (a.value("hideFromUser").toString().compare("true") == 0);
    critical = (a.value("critical").toString().compare("true") == 0);
    pointer = (a.value("pointer").toString().compare("true") == 0);
    dataType = DataType::getType(a.value("dataType").toString().toStdString());

    in.skipCurrentElement();
}

QXmlStreamWriter& operator<<(QXmlStreamWriter& out, CParameter& parameter)  {
    out.writeStartElement("parameter");
    out.writeAttribute("name", parameter.name.c_str());
    out.writeAttribute("value", parameter.value.c_str());
    out.writeAttribute("hideFromUser", parameter.hideFromUser ? "true" : "false");
    out.writeAttribute("critical", parameter.critical ? "true" : "false");
    out.writeAttribute("dataType", parameter.dataType->getName().c_str());
    out.writeAttribute("pointer", parameter.pointer ? "true" : "false");

    out.writeEndElement();
    return out;
}

QXmlStreamReader& operator>>(QXmlStreamReader& in, CParameter& parameter) {
    in.readNextStartElement();

    if (in.name().toString().compare("parameter") == 0) {
        parameter.loadFromXmlStream(in);
    } else {
        std::cerr << "unexpected name in XML reading" << std::endl;
    }

    return in;
}
