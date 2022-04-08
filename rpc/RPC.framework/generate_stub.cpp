#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <map>
#include <libgen.h>
#include "c150exceptions.h"
#include "c150debug.h"
#include "declarations.h"
#include "functiondeclaration.h"
#include "typedeclaration.h"

#include "idl_to_json.hpp"
#include "utility.hpp"

using namespace std;
using namespace C150NETWORK;

namespace C150NETWORK{
    int writeStubHeader(stringstream& output, const char idl_filename[]);
    int writeStructDefinitions(stringstream& output, const Declarations& parseTree);
    int writeStubTypeParsers(stringstream& output, const Declarations& parseTree);
    int writeStubFunctions(stringstream& output, const Declarations& parseTree);
    int writeStubBadFunction(stringstream& output, const char idl_filename[]);
    int writeStubDispatcher(stringstream& output, const Declarations& parseTree);
    int writeStubGetFunctionNameFromStream(stringstream& output, const char idl_filename[]);
    int writeStubReadFromStream(stringstream& output);
    int generateStub(const char idl_filename[], const char outputFilepath[]);
}

// left for test only, should not be compiled when not tested separately
// int main(int argc, char* argv[]) {
//     if (argc != 3) {
//         cout << "Usage: " << argv[0] << " <idl_filename> <output_filepath>" << endl;
//         return -1;
//     }
//     return generateStub(argv[1], argv[2]);
// }

namespace C150NETWORK {
    int generateStub(const char idl_filename[], const char outputFilepath[]) {
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

        try {
            if (parseTree.functions.size() == 0 && parseTree.types.size() == 0) {
                throw C150Exception("This idl file contains no functions or types");
            }
            if (writeStubHeader(stub_file, idl_filename) != 0) {
                throw C150Exception("Fail in writing stub header");
            }
            if (writeStructDefinitions(stub_file, parseTree) != 0) {
                throw C150Exception("Fail in writing stub struct definitions");
            }
            if (writeStubTypeParsers(stub_file, parseTree) != 0) {
                throw C150Exception("Fail in writing stub type parsers");
            }
            if (writeStubFunctions(stub_file, parseTree) != 0) {
                throw C150Exception("Fail in writing stub functions");
            }
            if (writeStubBadFunction(stub_file, idl_filename) != 0) {
                throw C150Exception("Fail in writing stub bad function");
            }
            if (writeStubDispatcher(stub_file, parseTree) != 0) {
                throw C150Exception("Fail in writing stub dispatcher");
            }
            if (writeStubGetFunctionNameFromStream(stub_file, idl_filename) != 0) {
                throw C150Exception("Fail in writing stub getFunctionNameFromStream");
            } 
            if (writeReadFromStream(stub_file) != 0) {
                throw C150Exception("Fail in writing stub readFromStream");
            }
        } catch (C150Exception& e) {
            c150debug->printf(C150APPLICATION, "Caught C150Exception: %s", e.formattedExplanation());
            cout << "Caught C150Exception: " << e.formattedExplanation() << endl;
            return -2;
        }

        string idl_filename_str(idl_filename, strlen(idl_filename) - 4);
        stringstream ss;
        if (outputFilepath == nullptr || strlen(outputFilepath) == 0) {
            ss << idl_filename_str << ".stub.cpp";
        } else {
            if (idl_filename_str.find_last_of('/') != string::npos) {
                idl_filename_str = idl_filename_str.substr(idl_filename_str.find_last_of('/') + 1);
            }
            ss << outputFilepath;
            if (outputFilepath[strlen(outputFilepath) - 1] != '/') {
                ss << '/';
            }
            ss << idl_filename_str << ".stub.cpp";
        }

        ofstream stubFile(ss.str());
        stubFile << stub_file.str();
        stubFile.close();
        return 0;
    }

    int writeStubHeader(stringstream& output, const char idl_filename[]) {
        try {
            if (idl_filename == nullptr) {
                throw C150Exception("write_header: idl_filename is null");
            }
        } catch (C150Exception& e) {
            c150debug->printf(C150APPLICATION, "Caught C150Exception: %s", e.formattedExplanation());
            cout << "Caught C150Exception: " << e.formattedExplanation() << endl;
            return -4;
        }      

        output << "// Language: cpp" << endl;
        output << "// Path: rpc/RPC.samples/generate_stub.cpp" << endl;
        output << "// Compare this snippet from rpc/RPC.samples/simplefunction.proxy.cpp:" << endl;
        output << endl;

        string idl_filename_string(idl_filename);
        if (idl_filename_string.find_last_of('/') != string::npos) {
            idl_filename_string = idl_filename_string.substr(idl_filename_string.find_last_of('/') + 1);
        }

        output << "#include \"" << idl_filename_string << "\"" << endl;
        output << endl;
        output << "#include \"rpcproxyhelper.h\"" << endl;
        output << "#include \"c150debug.h\"" << endl;
        output << "#include <string>" << endl;
        output << "#include \"base64.hpp\"" << endl;
        output << endl;
        output << "using namespace C150NETWORK;" << endl;
        output << endl;
        output << "void getFunctionNamefromStream(char *buffer, unsigned int bufSize);" << endl;
        output << "string readFromStream();" << endl;
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
    // todo deal with multi-dimensional arrays
    int writeStructDefinitions(stringstream& output, const Declarations& parseTree) {
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
                // } else if (structDeclaration.second->isArray()) {
                //     output << "// " << structDeclaration.second->getName() << endl;
                //     output << "struct " << structDeclaration.second->getName() << " {" << endl;
                //     output << "  " << structDeclaration.second->getArrayType()->getName() << " *data;" << endl;
                //     output << "  int size;" << endl;
                //     output << "};" << endl;
                //     output << endl;
                // }
            }
        }
        return 0;
    }

    int writeStubTypeParsers(stringstream& output, const Declarations& parseTree) {
        output << "string string64_to_string(string *val) {\n"
               << "  return base64_encode(*val);\n"
               << "}\n\n"
               << "void parse_string(string *value, string arg) {\n"
               << "  *value = arg;\n"
               << "}\n\n";
        output << "string string64_to_int(int *val) {\n"
               << "  return base64_encode(to_string(*val));\n"
               << "}\n\n"
               << "void parse_int(int *value, string arg) {\n"
               << "  *value = stoi(arg);\n"
               << "}\n\n"
               << "string string64_to_float(float *val) {\n"
               << "  return base64_encode(to_string(*val));\n"
               << "}\n\n";
        output << "void parse_float(float *value, string arg) {\n"
               << "  *value = stof(arg);\n"
               << "}\n\n";
        stringstream encDecl, decDecl;

        for (auto& type : parseTree.types) {
            if (type.first == "int" || type.first == "float" || type.first == "string" || type.first == "void") {
                continue;
            }
            auto& typeDecl = type.second;

            string val = "val";
            encDecl.clear();
            decDecl.clear();
            
            if (typeDecl->isArray()) {
                string typeName = typeDecl->getName();
                typeName = typeName.substr(typeName.find_last_of('_') + 1);
                for (size_t i = 0; i < typeName.size(); i++) {
                    if (typeName[i] == '[' || typeName[i] == ']') {
                        typeName[i] = '_';
                    }
                }

                string rawType(typeDecl->getName());
                string arrayType = rawType.substr(2, rawType.size() - 2);
                // cout << "array type: " << arrayType << endl;
                int idx = arrayType.find_first_of('[');
                string dataType = arrayType.substr(0, idx);
                string arrayIdx = arrayType.substr(idx, arrayType.size() - idx + 1);

                encDecl << "string string64_to_" << typeName << "(" << dataType << " val" << arrayIdx << ") {\n";
                decDecl << "void parse_" << typeName << "(" << dataType << " value" << arrayIdx << ", string arg" << ") {\n";
            } else {
                encDecl << "string string64_to_" << typeDecl->getName() << "(" << typeDecl->getName() << " *val) {\n";
                decDecl << "void parse_" << typeDecl->getName() << "(" << typeDecl->getName() << " *value, string arg) {\n";
                val = "*(val)";
            }

            encDecl << "  stringstream ss;\n";
            decDecl << "  stringstream args;\n  string arg64;\n  args.str(arg);\n";

            if (typeDecl->isStruct()) {
                for (auto& member: typeDecl->getStructMembers()) {
                    decDecl << "  args >> arg64;\n";
                    if (member->getType()->isArray()) {
                        encDecl << "  ss << " << getEncDecl(member->getType()) << "(" << val << "." << member->getName() << ") << ' ';\n";
                        decDecl << "  " << getDecDecl(member->getType()) << "((*val)." << member->getName() << ", base64_decode(arg64));\n";
                    } else {
                        encDecl << "  ss << " << getEncDecl(member->getType()) << "(&("  
                                << val << "." << member->getName() << ")) << ' ';\n";
                        decDecl << "   " << getDecDecl(member->getType()) << "(&(" << val << "." <<  member->getName() <<"), base64_decode(arg64));\n";
                    }
                }
            } else {
                encDecl << "  for(int i = 0; i < " << typeDecl->getArrayBound() << "; i++) {\n";
                if (typeDecl->isArray()) {
                    encDecl << "    ss << " << getEncDecl(typeDecl) << "(" << val << "[i]) << ' ';" << endl;
                }
                else {
                    encDecl << "    s << " << getEncDecl(typeDecl) << " << (&(" << val <<"[i])) << ' ';" << endl;
                }
                encDecl << "  }\n";

                decDecl << "  for(int i = 0; i < " << typeDecl->getArrayBound() << "; i++) {" << endl;
                decDecl << "    args >> arg64;" << endl;
                if (typeDecl->isArray()) {
                    decDecl << "    " << getDecDecl(typeDecl) << "(" << val << "[i], base64_decode(arg64));" << endl;
                }
                else {
                    decDecl << "    " << getDecDecl(typeDecl) << "(&(" << val << "[i]), base64_decode(arg64));" << endl;
                }
                decDecl << "  }" << endl;
            }

            encDecl << "  return base64_encode(ss.str());" << endl;
            encDecl << "}" << endl;

            decDecl << "}" << endl;
        }

        output << encDecl.str();
        output << endl;
        output << decDecl.str();
        return 0;
    }

    int writeStubFunctions(stringstream& output, const Declarations& parseTree) {
        if (parseTree.functions.size() == 0) {
            return 0;
        }

        for (auto& function: parseTree.functions) {
            auto& members = function.second->getArgumentVector();

            // declares the stub function and the inputstream to read arguments
            output << "void __" << function.first << "(string args64) {\n";
            output << "  c150debug->printf(C150RPCDEBUG, \"called " << function.first << "with %s\", args64.c_str());\n";
            output << "  *GRADING << \"stub: called __" << function.first << "with base64string of \" << args64 << endl;\"\n";
            output << "  stringstream args;\n";
            output << "  string arg64;\n";
            output << "  args.str(base64_decode(args64));\n";
            // names = []
            //  generates code to read each base64 encoded string from the argument string,
            //  it then parses the decoded argument into the correct memory location
            for (auto& member: members) {
                // names.append(a["name"])
                output << "  *GRADING << \"stub: parsing arg - " << member->getName() << "\" << endl;\n";
                if (member->getType()->isArray()) {
                    ArrayDecl decl = getArrayDecl(member->getType());
                    output << "  " << decl.type << " " << member->getName() << decl.idx << ";\n";
                } else {
                    output << "  " << member->getType()->getName() << " " << member->getName() << ";\n";
                }
                output << "  args >> arg64;\n";
                // arrays need to be passed by value into their parse functions because
                // the compiler didn't like it other ways
                if (member->getType()->isArray()) {
                    output << "  " << getDecDecl(member->getType()) << "(" << member->getName() << ", base64_decode(arg64));" << endl;
                } else {
                    output << "  " << getDecDecl(member->getType()) << "(&" << member->getName() << ", base64_decode(arg64));" << endl;
                }
            }
            output << "  string response = \"" << function.first << "\";\n";
            output << "  c150debug->printf(C150RPCDEBUG,\"stub: invoking "<< function.first << "()\");\n";
            // create the response string if necessary and call the function
            if (function.second->getReturnType()->getName() == "void") {
                output << "  " << function.first << "(";
                for (size_t i = 0; i < members.size(); i++) {
                    output << members[i]->getName();
                    if (i < members.size() - 1) {
                        output << ", ";
                    }
                }
                output << ");" << endl;
            } else {
                output << "  " << function.second->getReturnType()->getName() << " retval = " << function.first << "(";
                for (size_t i = 0; i < members.size(); i++) {
                    output << members[i]->getName();
                    if (i < members.size() - 1) {
                        output << ", ";
                    }
                }
                output << ");" << endl;
                output << "  response = response + ' ' + " << getEncDecl(function.second->getReturnType()) << "(&retval);\n";
            }
            // encode and return the results of the function to the clear
            output << "  c150debug->printf(C150RPCDEBUG,\"stub: " << function.first << "() has returned\");" << endl;
            output << "  *GRADING << \"stub: function " << function.first << " returned. Response to client - \" << response << endl;" << endl;
            output << "  RPCSTUBSOCKET->write(response.c_str(), strlen(response.c_str()) + 1);\n";
            output << "}" << endl << endl;
            /*
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
            output << ") {\n";
            output << "  char doneBuffer[5] = \"DONE\";  // to write magic value DONE + null\n";
            output << "  //\n";
            output << "  // Time to actually call the function \n";
            output << "  //\n";
            output << "  c150debug->printf(C150RPCDEBUG,\"simplefunction.stub.cpp: invoking " << function.first << "()\");\n";
            output << "  " << function.first << "(";
            for (size_t i = 0; i < members.size(); i++) {
                auto member = members[i];
                output << member->getName();
                if (i < members.size() - 1) {
                    output << ", ";
                }
            }
            output << ");\n";
            output << "  //\n";
            output << "  // Send the response to the client\n";
            output << "  //\n";
            output << "  // If " << function.first << " returned something other than void, this is\n";
            output << "  // where we'd send the return " << function.second->getReturnType()->getName() << " to the client\n";
            output << "  RPCSTUBSOCKET->write(doneBuffer, 5);\n";
            output << "}\n\n";
            */
        }  
        return 0;
    }

    int writeStubBadFunction(stringstream& output, const char idl_filename[]) {
        try {
            if (idl_filename == nullptr) {
                throw C150Exception("write_bad_function: output stream is null");
            }
        } catch (C150Exception& e) {
            c150debug->printf(C150APPLICATION,"%s",e.formattedExplanation());
            cout << "write_bad_function: " << e.formattedExplanation() << endl;
            return -6;
        } 

        string idl_filename_string(idl_filename, strlen(idl_filename) - 4);

        output << "// \n";
        output << "//     __badFunction \n";
        output << "//\n";
        output << "//   Pseudo-stub for missing functions. \n";
        output << "//\n";

        output << "void __badFunction(char *functionName) {\n";
        output << "  char doneBuffer[5] = \"BAD\";\n";
        output << "  c150debug->printf(C150RPCDEBUG,\"" << idl_filename_string << ".stub.cpp"  << ": received call for nonexistent function %s()\", functionName);\n";
        output << "  RPCSTUBSOCKET->write(doneBuffer, 5);\n";
        output << "}\n\n";

        return 0;
    }

    int writeStubDispatcher(stringstream& output, const Declarations& parseTree) {
        if (parseTree.functions.size() == 0) {
            // at this time there would be no more functions to write
            // simply wriye the bad function

            output << "void dispatchFunction() {\n";
            output << "  char functionNameBuffer[50];\n";
            output << "  getFunctionNameFromStream(functionNameBuffer,sizeof(functionNameBuffer));\n";
            output << "  if (!RPCSTUBSOCKET-> eof()) {\n";
            output << "    __badFunction();\n";
            output << "  }\n";
            output << "}\n";

            return 0;
        }

        auto fiter = parseTree.functions.begin();

        output << "// ======================================================================\n";
        output << "//                             DISPATCHER\n";
        output << "//\n";
        output << "//    This is the dispatcher for the stub. It reads the function name\n";
        output << "//    from the input stream, and then calls the appropriate routine.\n";
        output << "//\n";
        output << "// ======================================================================\n";
        output << endl;

        output << "void dispatchFunction() {\n";
        output << "  char functionNameBuffer[50];\n";
        output << "  //\n";
        output << "  // Read the function name from the stream -- note\n";
        output << "  // REPLACE THIS WITH YOUR OWN LOGIC DEPENDING ON THE\n"; 
        output << "  // WIRE FORMAT YOU USE\n";
        output << "  //\n";
        output << "  getFunctionNameFromStream(functionNameBuffer,sizeof(functionNameBuffer));\n";

        //
        // We've read the function name, call the stub for the right one
        // The stub will invoke the function and send response.
        //
        output << "  if (!RPCSTUBSOCKET-> eof()) {\n";
        output << "    if (strcmp(functionName, \"" << fiter->first << "\") == 0) {\n";
        output << "      __" << fiter->first << "();\n";
        for (++fiter; fiter != parseTree.functions.end(); ++fiter) {
            output << "    } else if (strcmp(functionName, \"" << fiter->first << "\") == 0) {\n";
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

    int writeStubGetFunctionNameFromStream(stringstream& output, const char idl_filename[]) {
        output << "// ======================================================================\n";
        output << "//           GET_FUNCTION_NAME_FROM_STREAM\n";
        output << "//\n";
        output << "//    This routine reads the variable from the input stream.\n";
        output << "//\n";
        output << "// ======================================================================\n";
        output << endl;

        output << "void getFunctionNamefromStream(char *buffer, unsigned int bufSize) {\n";
        output << "  unsigned int i;\n"
               << "  char bufc;    // next char to read\n"
               << "  stringstream ss;\n"
               << "  bool readnull;\n"
               << "  ssize_t readlen;             // amount of data read from socket\n";
        
        output << "  //\n";
        output << "  // Read a message from the stream.\n";
        output << "  // -1 in size below is to leave room for null\n";
        output << "  //\n"
               << "  readnull = false;\n"
               << "  bufp = buffer;\n"
               << "  for (i = 0; i < bufSize; i++) {\n"
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

        char * idl_filename_base = basename((char *)idl_filename);
        string idl_filename_string(idl_filename_base, strlen(idl_filename_base) - 4);

        output << "  //\n"
               << "  // With TCP streams, we should never get a 0 length read\n"
               << "  // except with timeouts (which we're not setting in pingstreamserver)\n"
               << "  // or EOF\n"
               << "  //\n";
        output << "  if (readlen == 0) {\n"
               << "    c150debug->printf(C150RPCDEBUG, \"" << idl_filename_string <<".stub.cpp: read zero length message, checking EOF\");\n";

        output << "    if (RPCSTUBSOCKET-> eof()) {\n"
               << "      c150debug->printf(C150RPCDEBUG, \"" << idl_filename_string << ".stub.cpp: EOF signaled on input\");\n"
               << "    } else {\n"
               << "      throw C150Exception(\"" << idl_filename_string << ".stub.cpp: unexpected zero length read without eof\");\n"
               << "    }\n"
               << "  }\n";

        output << "  //\n"
               << "  // If we didn't get a null, input message was poorly formatted\n"
               << "  //\n"
               << "  else if(!readnull) \n"
               << "    throw C150Exception(\"" << idl_filename_string << ".stub.cpp: method name not null terminated or too long\");\n"
               << "\n\n";
        
        output << "  //\n"
               << "  // Note that eof may be set here for our caller to check\n"
               << "  //\n"
               << "}\n";
        

        return 0;
    }

    int writeStubReadFromStream(stringstream& output) {
        output << "string readFromStream() {\n";
        output << "    stringstream name;          // name to build\n";
        output << "    char bufc;                  // next char to read\n";
        output << "    ssize_t readlen;            // amount of data read from socket\n\n";

        output << "    while(1) {\n";
        output << "        readlen = RPCSTUBSOCKET-> read(&bufc, 1);  // read a byte\n";

        output << "        //\n";
        output << "        // With TCP streams, we should never get a 0 length read except with\n";
        output << "        // timeouts (which we're not setting in pingstreamserver) or EOF\n";
        output << "        //\n";
        output << "        if (readlen == 0) { \n";
        output << "            c150debug->printf(C150RPCDEBUG,\"stub: read zero length message, checking EOF\");\n";
        output << "            if (RPCSTUBSOCKET-> eof()) {\n";
        output << "                c150debug->printf(C150RPCDEBUG, \"stub: EOF signaled on input\");\n";
        output << "                return name.str();\n";
        output << "            } else { \n";
        output << "                throw C150Exception(\"stub: unexpected zero length read without eof\");\n";
        output << "            }\n";
        output << "        }\n\n";

        output << "        // check for null or space\n";
        output << "        if (bufc == '\0') {\n";
        output << "            return name.str();\n";
        output << "        }\n";
        output << "        name << bufc;\n";
        output << "    }\n";
        output << "    throw C150Exception(\"readFromString: This should never be thrown.\");\n";
        output << "}\n"

        return 0;
    }
}
