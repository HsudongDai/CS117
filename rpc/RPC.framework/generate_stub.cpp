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

int write_header(stringstream& output, const char idl_filename[]) {
    if (idl_filename == nullptr) {
        throw C150Exception("write_header: dil_filename is null");
        return -1;
    }

    output << "// Language: cpp" << endl;
    output << "// Path: rpc/RPC.samples/generate_stub.cpp" << endl;
    output << "// Compare this snippet from rpc/RPC.samples/simplefunction.proxy.cpp:" << endl;
    output << endl;
    output << "#include \"" << idl_filename << "\"" << endl;
    output << endl;
    output << "#include \"rpcproxyhelper.h\"" << endl;
    output << endl;
    output << "using namespace C150NETWORK;" << endl;
    output << endl;
    output << "void getFunctionNamefromStream();" << endl;
    output << endl;

    output << "// ======================================================================\n";
    output << "//                             STUBS\n";
    output << "//\n";
    output << "//    Each of these is generated to match the signature of the \n";
    output << "//    function for which it is a stub. The names are prefixed with \"__\"\n";
    output << "//    to keep them distinct from the actual routines, which we call!\n";
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

int write_functions(stringstream& output, const Declarations& parseTree) {
    if (parseTree.functions.size() == 0) {
        throw C150Exception("write_functions: output stream is null");
        return -1;
    }

    for (auto& function : parseTree.functions) {
        output << "void __" << function.first << "() {\n";
        output << "  char doneBuffer[5] = \"DONE\";  // to write magic value DONE + null\n";
        output << "  //\n";
        output << "  // Time to actually call the function \n";
        output << "  //\n";
        output << "  c150debug->printf(C150RPCDEBUG,\"simplefunction.stub.cpp: invoking " << function.first << "()\");\n";
        output << "  " << function.first << "();\n";
        output << "  //\n";
        output << "  // Send the response to the client\n";
        output << "  //\n";
        output << "  // If " << function.first << " returned something other than void, this is\n";
        output << "  // where we'd send the return" << function.second->getReturnType() << " to the client\n";
        output << "  RPCSTUBSOCKET->write(doneBuffer, 5);\n";
        output << "}\n\n";
    }  
    return 0;
}

int write_bad_function(stringstream& output, const char idl_filename[]) {
    if (idl_filename == nullptr) {
        throw C150Exception("write_bad_function: output stream is null");
        return -1;
    }
    string idl_filename_string(idl_filename, strlen(idl_filename) - 4);

    output << "  // \n";
    output << "  //     __badFunction \n";
    output << "  //\n";
    output << "  //   Pseudo-stub for missing functions. \n";
    output << "  //\n";

    output << "void __badFunction(char *functionName) {\n";
    output << "  char doneBuffer[5] = \"BAD\";\n";
    output << "  c150debug->printf(C150RPCDEBUG,\"" << idl_filename_string << ".stub.cpp"  << ": received call for nonexistent function %%s()\", functionName);\n";
    output << "  RPCSTUBSOCKET->write(doneBuffer, 5);\n";
    output << "}\n";

    return 0;
}

int write_dispatcher(stringstream& output, const Declarations& parseTree) {
    if (parseTree.functions.size() == 0) {
        throw C150Exception("write_dispatcher: output stream is null");
        return -1;
    }

    auto fiter = parseTree.functions.begin();
    // FunctionDeclaration *functionp;    // Declare FunctionDelcaration pointer

    output << "// ======================================================================\n";
    output << "//                             DISPATCHER\n";
    output << "//\n";
    output << "//    This is the dispatcher for the stub. It reads the function name\n";
    output << "//    from the input stream, and then calls the appropriate routine.\n";
    output << "//\n";
    output << "// ======================================================================\n";
    output << endl;

    output << "void getFunctionNamefromStream() {\n";
    output << "  char functionName[50];\n";
    output << "  RPCSTUBSOCKET->read(functionName, 50);\n";
    output << "  if (!RPCSTUBSOCKET-> eof()) {\n";
    output << "    if (strcmp(functionName, " << fiter->first << ") == 0) {\n";
    output << "      __" << fiter->first << "();\n";
    for (; fiter != parseTree.functions.end(); ++fiter) {
        output << "    } else if (strcmp(functionName, " << fiter->first << ") == 0) {\n";
        output << "      __" << fiter->first << "();\n";
    }
    output << "    } else {\n";
    output << "      __badFunction();\n";
    output << "    }\n";
    output << "  }\n";
    output << "}\n";
    output << endl;

    return 0;
}

int write_get_function_name_from_stream(stringstream& output,  const char idl_filename[]) {
    if (idl_filename == nullptr) {
        throw C150Exception("write_get_function_name_from_stream: output stream is null");
        return -1;
    }

    output << "// ======================================================================\n";
    output << "//                   GET_FUNCTION_NAME_FROM_STREAM\n";
    output << "//\n";
    output << "//    This routine reads the function name from the input stream.\n";
    output << "//\n";
    output << "// ======================================================================\n";
    output << endl;

    output << "void getFunctionNameFromStream(char *buffer, unsigned int bufSize) {\n";

    output << "  unsigned int i;\n"
           << "  char *bufp;    // next char to read\n"
           << "  bool readnull;\n"
           << "  ssize_t readlen;             // amount of data read from socket\n";
    
    output << "  //\n";
    output << "  // Read a message from the stream.\n";
    output << "  // -1 in size below is to leave room for null\n";
    output << "  //\n"
           << "  readnull = false;\n"
           << "  bufp = buffer;\n"
           << "  for (i=0; i< bufSize; i++) {\n"
           << "    readlen = RPCSTUBSOCKET-> read(bufp, 1);  // read a byte\n"
           << "    // check for eof or error\n"
           << "    if (readlen == 0) {\n"
           << "      break;\n";
    output << "    }\n"
           << "    // check for null and bump buffer pointer\n"
           << "    if (*bufp++ == '\\0') {\n"
           << "      readnull = true;\n"
           << "      break;\n"
           << "    }\n"
           << "  }\n";

    string idl_filename_string(idl_filename, strlen(idl_filename) - 4);

    output << "  //\n"
           << "  // With TCP streams, we should never get a 0 length read\n"
           << "  // except with timeouts (which we're not setting in pingstreamserver)\n"
           << "  // or EOF\n"
           << "  //\n";
    output << "  if (readlen == 0) {\n"
           << "    c150debug->printf(C150RPCDEBUG," << idl_filename_string <<".stub: read zero length message, checking EOF);\n";

    output << "    if (RPCSTUBSOCKET-> eof()) {\n"
           << "      c150debug->printf(C150RPCDEBUG," << idl_filename_string << ".stub: EOF signaled on input);\n"
           << "    } else {\n"
           << "      throw C150Exception(" << idl_filename_string << ".stub: unexpected zero length read without eof);\n"
           << "    }\n"
           << "  }\n";

    output << "  //\n"
           << "  // If we didn't get a null, input message was poorly formatted\n"
           << "  //\n"
           << "  else if(!readnull) \n"
           << "    throw C150Exception(" << idl_filename_string << ".stub: method name not null terminated or too long);\n"
           << "\n\n";
    
    output << "  //\n"
           << "  // Note that eof may be set here for our caller to check\n"
           << "  //\n"

           << "}\n";

    return 0;
}

int generate_stub(const char idl_filename[]) {
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
    stringstream stub_file;
    write_header(stub_file, idl_filename);
    write_functions(stub_file, parseTree);
    write_bad_function(stub_file, idl_filename);
    write_dispatcher(stub_file, parseTree);
    write_get_function_name_from_stream(stub_file, idl_filename);

    string stub_filename(idl_filename, strlen(idl_filename) - 4);
    stub_filename += ".stub.cpp";
    ofstream stubFile(stub_filename.c_str());
    stubFile << stub_file.str();
    stubFile.close();
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " <idl_filename>" << endl;
        return -1;
    }
    return generate_stub(argv[1]);
}