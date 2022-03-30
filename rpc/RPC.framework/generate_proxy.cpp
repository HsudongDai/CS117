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

int writeProxyHeader(stringstream& output, const char idl_filename[]);
int writeProxyFunctions(stringstream& output, const Declarations& parseTree, const char idl_filename[]);
int generateProxy(const char idl_filename[]);

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " <idl_filename>" << endl;
        return -1;
    }

    generateProxy(argv[1]);
    return 0;
}

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
            output << member->getType()->getName() << " " << member->getName();
            if (i < members.size() - 1) {
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