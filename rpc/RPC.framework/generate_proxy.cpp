#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <map>
#include "c150exceptions.h"
#include "declarations.h"
#include "functiondeclaration.h"
#include "typedeclaration.h"

#include "idl_to_json.cpp"

using namespace std;

namespace C150NETWORK {
    int writeProxyHeader(stringstream& output, const char idl_filename[]);
    int writeProxyStructDefinitions(stringstream& output, const Declarations& parseTree);
    int writeProxyFunctions(stringstream& output, const Declarations& parseTree, const char idl_filename[]);
    int generateProxy(const char idl_filename[]); 
}


int main(int argc, char* argv[]) {
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " <idl_filename>" << endl;
        return -1;
    }

    generateProxy(argv[1]);
    return 0;
}

namespace C150NETWORK {
    int generateProxy(const char idl_filename[]) {
        ifstream idlFile(idl_filename);        // open 

        if (!idlFile.is_open()) {
            stringstream ss;
            ss << "Could not open IDL file: " << idl_filename;
            throw C150Exception(ss.str());
            return -2;
        }
        // The following line does all the work parsing the file into
        // the variable parseTree

        Declarations parseTree(idlFile);
        stringstream output;
        if (writeProxyHeader(output, idl_filename) != 0) {
            return -1;
        }
        if (writeProxyFunctions(output, parseTree, idl_filename) != 0) {
            return -1;
        }

        string proxy_filename(idl_filename, strlen(idl_filename) - 4);
        proxy_filename += ".proxy.cpp";
        ofstream proxyFile(proxy_filename.c_str());
        proxyFile << output.str();
        proxyFile.close();
        return 0;
    }

    int writeProxyHeader(stringstream& output, const char idl_filename[]) {
        output << "// Language: cpp" << endl;
        output << "// Path: rpc/RPC.samples/generate_proxy.cpp" << endl;
        output << "// Compare this snippet from rpc/RPC.samples/simplefunction.proxy.cpp:" << endl;
        output << endl;
        output << "#include \"rpcproxyhelper.h\"" << endl;
        output << endl;
        output << "#include \"" << idl_filename << "\"" << endl;
        output << endl;
        output << "#include <cstdio>" << endl;
        output << "#include <cstring>" << endl;
        output << "#include \"c150debug.h\"" << endl;
        output << "using namespace C150NETWORK;" << endl;
        output << endl;

        output << "// ======================================================================\n";
        output << "//                             PROXY\n";
        output << "//\n";
        output << "//    Each of these is generated to match the signature of the \n";
        output << "//    function for which it is a proxy. The names are the called ones.\n";
        output << "//\n";
        output << "//    Note that when functions take arguments, these are the routines\n";
        output << "//    that will read them from the input stream. These routines should\n";
        output << "//    be generated dynamically from your rpcgen program (and so should the\n";
        output << "//    code above).\n";
        output << "//\n";
        output << "// ======================================================================\n";
        output << endl;

        return 0;
    }

    int writeProxyStructDefinitions(stringstream& output, const Declarations& parseTree) {
        if (parseTree.types.size() == 0) {
            return 0;
        }

        output << "// ======================================================================\n";
        output << "//                             STRUCTS\n";
        output << "//\n";
        output << "//    These are generated to match the structs in your idl file.\n";
        output << "//    The names are prefixed with \"__\" to keep them distinct from\n";
        output << "//    the actual routines, which we call!\n";
        output << "//\n";
        output << "// ======================================================================\n";
        output << endl;

        for (auto& structDeclaration : parseTree.types) {
            if (structDeclaration.second->isStruct()) {
                output << "// " << structDeclaration.second->getName() << endl;
                output << "struct " << structDeclaration.second->getName() << " {" << endl;
                for (auto& field : structDeclaration.second->getStructMembers()) {
                    if (field->getType()->isArray()) {
                        string rawType(field->getType()->getName());
                        string arrayType = rawType.substr(2, rawType.size() - 2);
                        // cout << "array type: " << arrayType << endl;
                        int idx = arrayType.find("[");
                        string dataType = arrayType.substr(0, idx);
                        string arrayIdx = arrayType.substr(idx, arrayType.size() - idx + 1);
                        output << "  " << dataType << " " << field->getName() << arrayIdx << ";" << endl;
                        // output << "  " << arrayType << " " << field->getName() << "[" << field->getType()->getArrayBound() << "];" << endl;
                    } else {
                        output << "  " << field->getType()->getName() << " " << field->getName() << ";" << endl;
                    }          
                }
                output << "};" << endl;
                output << endl;
            }
        }
        return 0;
    }

    // todo: must deal with array-type parameters
    int writeProxyFunctions(stringstream& output, const Declarations& parseTree, const char idl_filename[]) {
        if (parseTree.functions.size() == 0) {
            throw C150Exception("write_functions: output stream is null");
            return -1;
        }

        string idl_filename_string(idl_filename, strlen(idl_filename) - 4);

        for (auto function : parseTree.functions) {
            output << "// " << function.first << endl;
            output << function.second->getReturnType()->getName() << " " << function.first << "(";
            auto members = function.second->getArgumentVector();
            for (size_t i = 0; i < members.size(); i++) {
                auto member = members[i];
                if (member->getType()->isArray()) {
                    string rawType(member->getType()->getName());
                    string arrayType = rawType.substr(2, rawType.size() - 2);
                    // cout << "array type: " << arrayType << endl;
                    int idx = arrayType.find("[");
                    string dataType = arrayType.substr(0, idx);
                    string arrayIdx = arrayType.substr(idx, arrayType.size() - idx + 1);
                    output << dataType << " " << member->getName() << arrayIdx;
                } else {
                    output << member->getType()->getName() << " " << member->getName();
                }
                if (i != members.size() - 1) {
                    output << ", ";
                }
            }
            output << ") {" << endl;
            output << "  char readBuffer[5];  // to read magic value DONE + null\n";
            output << "  //\n";
            output << "  // Send the Remote Call\n";
            output << "  //\n";
            output << "  c150debug->printf(C150RPCDEBUG, \"" << idl_filename_string << ".proxy.cpp:" << function.first << "invoked\");\n";
            output << "  RPCPROXYSOCKET->write(" << function.first << ", strlen(" << function.first << ")+1); // write function name including null\n";
            output << "  //\n";
            output << "  // Read the response\n";
            output << "  //\n";
            output << "  c150debug->printf(C150RPCDEBUG, \"" << idl_filename_string << ".proxy.cpp: " << function.first << "() invocation sent, waiting for response\");\n";
            output << "  RPCPROXYSOCKET->read(readBuffer, sizeof(readBuffer)); // only legal response is DONE\n";

            output << "  //\n";
            output << "  // Check the response\n";
            output << "  //\n";
            output << "  if (strncmp(readBuffer, \"DONE\", sizeof(readBuffer)) != 0) {\n";
            output << "    throw C150Exception(\"" << idl_filename_string << ".proxy: " << function.first << "() received invalid response from the server\");\n";
            output << "  }\n";
            output << "  c150debug->printf(C150RPCDEBUG, \"" << idl_filename_string << ".proxy.cpp: " << function.first << " () successful return from remote call\");\n";
            output << "}" << endl;
            output << endl;
        }

        return 0;
    }
}