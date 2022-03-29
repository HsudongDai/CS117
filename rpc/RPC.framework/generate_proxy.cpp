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

int writeProxyHeader(stringstream& output, const char idl_filename[]) {
    output << "// Language: cpp" << endl;
    output << "// Path: rpc/RPC.samples/generate_proxy.cpp" << endl;
    output << "// Compare this snippet from rpc/RPC.samples/simplefunction.proxy.cpp:" << endl;
    output << endl;
    output << "#include \"rpcproxyhelper.h\"" << endl;
    output << endl;
    output << "#incldue \"" << idl_filename << "\"" << endl;
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
        output << function.second->getReturnType() << " " << function.first << "(";
        for (auto param = function.second->getArgumentVector().begin(); param != function.second->getArgumentVector().end(); param++) {
            output << param.second->getType() << " " << param.first;
            if (param != function.second->getArgumentVector().end() - 1) {
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
        output << "  throw C150Exception(" << idl_filename_string << ".proxy: " << function.first << "() received invalid response from the server\");\n";
        output << "  }\n";
        output << " c150debug->printf(C150RPCDEBUG, \"" << idl_filename_string << ".proxy.cpp: " <<  << " () successful return from remote call\");\n";
        output << "}" << endl;
        output << endl;
    }

    return 0;
}