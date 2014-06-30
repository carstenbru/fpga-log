#include "cmethod.h"

using namespace std;

CMethod::CMethod(std::string name, CParameter returnType, string headerFile) :
    name(name),
    returnType(returnType),
    headerFile(headerFile)
{
}

CMethod::~CMethod() {
//    delete returnType;
//    for (list<CParameter*>::iterator i = parameters.begin(); i != parameters.end(); i++) {
//        delete *i;
//    }
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

