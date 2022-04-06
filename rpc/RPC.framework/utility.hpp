#ifndef __UTILITY_HPP_INCLUDED__
#define __UTILITY_HPP_INCLUDED__

#include "declarations.h"
#include "functiondeclaration.h"
#include "typedeclaration.h"
#include <string>

namespace C150NETWORK {
    struct ArrayDecl {
        string type;
        string idx;
    };

    string replaceBrackets(string s);
    string removeUnderlines(string s);
    ArrayDecl getArrayDecl(TypeDeclaration* type);
    string getEncDecl(TypeDeclaration* type);
    string getDecDecl(TypeDeclaration* type);
}

#endif