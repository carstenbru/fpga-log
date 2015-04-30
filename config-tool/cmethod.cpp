#include "cmethod.h"
#include "cobject.h"

using namespace std;

CMethod::CMethod(std::string completeName, std::string name, CParameter returnType, string headerFile) :
    completeName(completeName),
    name(name),
    headerFile(headerFile),
    returnType(returnType),
    hideFromUser(false)
{
}

CMethod::CMethod(QXmlStreamReader& in, CMethod* currentSignature, CObject *object) {
    completeName = in.attributes().value("completeName").toString().toStdString();
    name = in.attributes().value("name").toString().toStdString();
    headerFile = in.attributes().value("headerFile").toString().toStdString();
    in >> returnType;

    unsigned int paramCount = 0;
    bool paramsChanged = false;
    std::list<CParameter>* sigParameters = currentSignature->getParameters();
    parameters.insert(parameters.end(), sigParameters->begin(), sigParameters->end());
    while (in.readNextStartElement()) {
        paramCount++;
        CParameter param = CParameter(in);
        if (!setParameter(param)) {
            paramsChanged = true;
        }
    }
    if (paramCount != parameters.size()) {
        paramsChanged = true;
    }
    if (paramsChanged) {
        object->setDefinitionsUpdated(true);
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

bool CMethod::setParameter(CParameter& newValue) {
    for (list<CParameter>::iterator i = parameters.begin(); i != parameters.end(); i++) {
        if ((*i).sameSignature(newValue) && ((*i).getName().compare(newValue.getName()) == 0)) {
            i = parameters.erase(i);
            parameters.insert(i, newValue);
            return true;
        }
    }
    return false;
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

    out.writeAttribute("completeName", cMethod.completeName.c_str());
    out.writeAttribute("name", cMethod.name.c_str());
    out.writeAttribute("headerFile", cMethod.headerFile.c_str());
    out << cMethod.returnType;

    for (list<CParameter>::iterator i = cMethod.parameters.begin(); i != cMethod.parameters.end(); i++) {
        out << *i;
    }

    out.writeEndElement();
    return out;
}
