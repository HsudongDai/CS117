#include <string>
#include "declarations.h"
#include "functiondeclaration.h"
#include "typedeclaration.h"
#include "base64.h"
#include "utility.h"

namespace C150NETWORK {
    
    string removeUnderlines(string s) const {
        int idx = s.find_first_not_of('_');
        return s.substr(idx + 1);
    }

    string replaceBrackets(string s) const {
        for (size_t i = 0; i < s.size(); ++i) {
            if (s[i] == '[' || s[i] == ']') {
                s[i] = '_';
            }
        }
        return s;
    }

    ArrayDecl getArrayDecl(TypeDeclaration* type) const {
        string rawType(typeDecl->getName());
        string arrayType = rawType.substr(2, rawType.size() - 2);
        // cout << "array type: " << arrayType << endl;
        int idx = arrayType.find_first_of('[');
        string dataType = arrayType.substr(0, idx);
        string arrayIdx = arrayType.substr(idx, arrayType.size() - idx + 1);

        return ArrayDecl{dataType, arrayIdx};
    }

    string getEncDecl(TypeDeclaration* type) const {
        if (type->isArray()) {
            ArrayDecl arrayDecl = getArrayDecl(type->getName());
            string decl = "string64_to_" + arrayDecl.type + replaceBrackets(arrayDecl.idx);
            return decl
        }
        string decl = "string64_to_" + type->getName();
        return decl;
    }

    string getDecDecl(TypeDeclaration* type) const {
        if (type->isArray()) {
            ArrayDecl arrayDecl = getArrayDecl(type->getName());
            string decl = "parse_" + arrayDecl.type + replaceBrackets(arrayDecl.idx);
            return decl;
        }
        string decl = "parse_" + type->getName();
        return decl;
    }
}