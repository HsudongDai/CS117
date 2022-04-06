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

    string replaceBrackets(string s) const;
    string removeUnderlines(string s) const;
    ArrayDecl getArrayDecl(TypeDeclaration* type) const;
    string getEncDecl(TypeDeclaration* type) const;
    string getDecDecl(TypeDeclaration* type) const;
}

#endif