#include "cmethod.h"

using namespace std;

CMethod::CMethod(std::string name, CParameter returnType, string headerFile) :
    name(name),
    headerFile(headerFile),
    returnType(returnType),
    hideFromUser(false)
{
}

CMethod::CMethod(QXmlStreamReader& in) {
    name = in.attributes().value("name").toString().toStdString();
    headerFile = in.attributes().value("headerFile").toString().toStdString();
    in >> returnType;
    while (in.readNextStartElement()) {
        parameters.push_back(CParameter(in));
    }
}

CMethod::~CMethod() {
}

bool CMethod::sameSignature(CMethod &compare) {
    if (!(returnType.sameSignature(compare.returnType)))
        return false;
    if (parameters.size() != compare.parameters.size())
        return 0;
    list<CParameter>::iterator comp_i = compare.parameters.begin();
    for (list<CParameter>::iterator i = parameters.begin(); i != parameters.end(); i++) {
        if (!((*i).sameSignature((*comp_i))))
            return false;
        comp_i++;
    }

    return true;
}

CParameter* CMethod::getParameter(std::string name) {
    for (list<CParameter>::iterator i = parameters.begin(); i != parameters.end(); i++) {
        if ((*i).getName().compare(name) == 0)
            return &*i;
    }
    return NULL;
}

list<CParameter*> CMethod::getMethodParameterPointers() {
    list<CParameter*> res;
    for (list<CParameter>::iterator i = ++parameters.begin(); i != parameters.end(); i++) {
        res.push_back(&*i);
    }
    return res;
}

QXmlStreamWriter& operator<<(QXmlStreamWriter& out, CMethod& cMethod) {
    out.writeStartElement("CMethod");

    out.writeAttribute("name", cMethod.name.c_str());
    out.writeAttribute("headerFile", cMethod.headerFile.c_str());
    out << cMethod.returnType;

    for (list<CParameter>::iterator i = cMethod.parameters.begin(); i != cMethod.parameters.end(); i++) {
        out << *i;
    }

    out.writeEndElement();
    return out;
}
