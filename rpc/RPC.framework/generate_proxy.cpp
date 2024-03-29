#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <map>
#include <libgen.h>
#include "c150debug.h"
#include "c150exceptions.h"
#include "declarations.h"
#include "functiondeclaration.h"
#include "typedeclaration.h"
#include "base64.hpp"

#include "utility.hpp"
#include "idl_to_json.hpp"

using namespace std;

namespace C150NETWORK {
    // all these functions are used to generate a separate part of the proxy file
    // just like their names suggest. They are highly independent of each other.
    // You can combine them to generate a complete proxy file.
    int writeProxyHeader(stringstream& output, const char idl_filename[]);
    int writeProxyStructDefinitions(stringstream& output, const Declarations& parseTree);
    int writeProxyTypeParsers(stringstream& output, const Declarations& parseTree);
    int writeProxyFunctions(stringstream& output, const Declarations& parseTree, const char idl_filename[]);
    int writeProxyReadFromStream(stringstream& output);

    // if you don't want to use manually, this function will generate the proxy file
    // by one click.
    int generateProxy(const char idl_filename[], const char outputFilepath[]); 
}

// for test-purpose only
// int main(int argc, char* argv[]) {
//     if (argc != 3) {
//         cout << "Usage: " << argv[0] << " <idl_filename> <output_filepath>" << endl;
//        return -1;
//     }

//     generateProxy(argv[1], argv[2]);
//     return 0;
// }

namespace C150NETWORK {
    int generateProxy(const char idl_filename[], const char outputFilepath[]) {
        ifstream idlFile(idl_filename);        // open the idl file

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

        try {
            if (parseTree.functions.size() == 0 && parseTree.types.size() == 0) {
                throw C150Exception("This idl file contains no functions or types");
            }
            if (writeProxyHeader(output, idl_filename) != 0) {
                throw C150Exception("Fail in writing proxy header");
            }
            // if (writeProxyStructDefinitions(output, parseTree) != 0) {
            //     throw C150Exception("Fail in writing proxy struct definitions");
            // }
            if (writeProxyTypeParsers(output, parseTree) != 0) {
                throw C150Exception("Fail in writing proxy type parsers");
            }
            if (writeProxyFunctions(output, parseTree, idl_filename) != 0) {
                throw C150Exception("Fail in writing proxy functions");
            }
            if (writeProxyReadFromStream(output) != 0) {
                throw C150Exception("Fail in writing proxy readFromStream");
            }
        } catch (C150Exception& e) {
            c150debug->printf(C150APPLICATION, "Caught C150Exception: %s", e.formattedExplanation());
            printf("Caught C150Exception: %s", e.formattedExplanation().c_str()	);
            return -3;
        }       

        // this is to remove the extension from the filename
        string idl_filename_str(idl_filename, strlen(idl_filename) - 4);
        stringstream ss;

        // generate the output file name
        // when the output file path is not specified, the output file is the same as the idl file but with a .proxy.cpp extension
        if (outputFilepath == nullptr || strlen(outputFilepath) == 0) {
            ss << idl_filename << ".proxy.cpp";
        }
        // else we need to put the path together with the filename
        else {
            if (idl_filename_str.find_last_of('/') != string::npos) {
                idl_filename_str = idl_filename_str.substr(idl_filename_str.find_last_of('/') + 1);
            }
            ss << outputFilepath;
            if (outputFilepath[strlen(outputFilepath) - 1] != '/') {
                ss << '/';
            }
            ss << idl_filename_str << ".proxy.cpp";
        }

        ofstream proxyFile(ss.str());
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
        string idl_filename_string(idl_filename);
        if (idl_filename_string.find_last_of('/') != string::npos) {
            idl_filename_string = idl_filename_string.substr(idl_filename_string.find_last_of('/') + 1);
        }
        output << "#include \"" << idl_filename_string << "\"" << endl;
        output << endl;
        output << "#include <cstdio>" << endl;
        output << "#include <cstring>" << endl;
        output << "#include <string>" << endl;
        output << "#include \"c150debug.h\"" << endl;
        output << "#include \"base64.hpp\"" << endl;
        output << endl;
        output << "using namespace C150NETWORK;" << endl;
        output << endl;
        output << "string readFromStream();" << endl << endl;

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

    int writeProxyTypeParsers(stringstream& output, const Declarations& parseTree) {
        // write the declartions first to deal with dependencies
        output << "string string64_to_string(string *val) {\n"
               << "  return base64_encode(*val);\n"
               << "}\n\n"
               << "string string64_to_int(int *val) {\n"
               << "  return base64_encode(to_string(*val));\n"
               << "}\n\n"
               << "string string64_to_float(float *val) {\n"
               << "  return base64_encode(to_string(*val));\n"
               << "}\n\n";
        output << "void parse_string(string *value, string arg) {\n"
               << "  *value = arg;\n"
               << "}\n\n"
               << "void parse_int(int *value, string arg) {\n"
               << "  *value = stoi(arg);\n"
               << "}\n\n"
               << "void parse_float(float *value, string arg) {\n"
               << "  *value = stof(arg);\n"
               << "}\n\n";
        stringstream encDecl, decDecl;

        stringstream encHead, decHead;

        for (auto& type : parseTree.types) {
            if (type.first == "int" || type.first == "float" || type.first == "string" || type.first == "void") {
                continue;
            }
            auto& typeDecl = type.second;

            string val = "val";
            encDecl.clear();
            decDecl.clear();
            encHead.clear();
            decHead.clear();
            
            // if the type is an array, we need to replace the square brackets with
            // a legal character, here we use the '_'
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
                encHead << "string string64_to_" << typeName << "(" << dataType << " val" << arrayIdx << ");\n";

                decDecl << "void parse_" << typeName << "(" << dataType << " val" << arrayIdx << ", string arg" << ") {\n";
                decHead << "void parse_" << typeName << "(" << dataType << " val" << arrayIdx << ", string arg" << ");\n";
            } else {
                encDecl << "string string64_to_" << typeDecl->getName() << "(" << typeDecl->getName() << " *val) {\n";
                encHead << "string string64_to_" << typeDecl->getName() << "(" << typeDecl->getName() << " *val);\n";

                decDecl << "void parse_" << typeDecl->getName() << "(" << typeDecl->getName() << " *val, string arg) {\n";
                decHead << "void parse_" << typeDecl->getName() << "(" << typeDecl->getName() << " *val, string arg);\n";

                val = "(*val)";
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
                if (typeDecl->getArrayMemberType()->isArray()) {
                    encDecl << "    ss << " << getEncDecl(typeDecl->getArrayMemberType()) << "(" << val << "[i]) << ' ';" << endl;
                }
                else {
                    encDecl << "    ss << " << getEncDecl(typeDecl->getArrayMemberType()) << "(&(" << val << "[i])) << ' ';" << endl;
                }
                encDecl << "  }\n";

                decDecl << "  for(int i = 0; i < " << typeDecl->getArrayBound() << "; i++) {" << endl;
                decDecl << "    args >> arg64;" << endl;
                if (typeDecl->getArrayMemberType()->isArray()) {
                    decDecl << "    " << getDecDecl(typeDecl->getArrayMemberType()) << "(" << val << "[i], base64_decode(arg64));" << endl;
                }
                else {
                    decDecl << "    " << getDecDecl(typeDecl->getArrayMemberType()) << "(&(" << val << "[i]), base64_decode(arg64));" << endl;
                }
                decDecl << "  }" << endl;
            }

            encDecl << "  return base64_encode(ss.str());" << endl;
            encDecl << "}" << endl << endl;

            decDecl << "}" << endl << endl;
        }
        
        output << encHead.str() << endl;
        output << decHead.str() << endl;

        output << encDecl.str();
        output << endl;
        output << decDecl.str();
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
            // throw C150Exception("write_functions: output stream is null");
            return 0;
        }

        char * idl_filename_base = basename((char *)idl_filename);
        string idl_filename_string(idl_filename_base, strlen(idl_filename_base) - 4);

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
                    int idx = arrayType.find_first_of('[');
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
            output << "  *GRADING << \"proxy: Called " << function.first << "\";\n" << endl;
            output << "  stringstream args;" << endl;
            output << "  args << ' ';"  << endl;

            for (auto& arg: function.second->getArgumentVector()) {
                output << "  *GRADING << \"proxy: encoding argument \" << \"" << arg->getName() << "\" << endl;" << endl;
                if (arg->getType()->isArray()) {
                    output << "  args << " << getEncDecl(arg->getType()) << "(" << arg->getName() << ")" << " << ' ';" << endl;
                } else {
                    output << "  args << " << getEncDecl(arg->getType()) << "(&" << arg->getName() << ")" << " << ' ';" << endl;
                }
            }

            output << "  string resp = \"" << function.first << " \" + base64_encode(args.str());" << endl;
            output << "  RPCPROXYSOCKET->write(resp.c_str(), strlen(resp.c_str()) + 1);" << endl;
            output << "  *GRADING << \"proxy: sending to client \"  << resp;" << endl;
            output << "  c150debug->printf(C150RPCDEBUG, \"proxy:" << function.first << " invoked.\");" << endl;
            output << "  // reads the response from the socket." << endl;
            output << "  stringstream ret;" << endl;
            output << "  string raw = readFromStream();" << endl;
            output << "  ret.str(raw);" << endl;
            output << "  string name;" << endl;
            output << "  ret >> name;" << endl;
            output << "  if (name != \"" << function.first << "\") {" << endl;
            output << "    *GRADING << \"proxy: Invalid response from server in !\" << name;" << endl; 
            output << "  }" << endl;
            output << "  // parses the return value if necessary" << endl;
            if (function.second->getReturnType()->getName() != "void") {
                output << "  string msg;" << endl;
                output << "  ret >> msg;" << endl;
                output << "  *GRADING << \"proxy: function " << function.first << "returned with - \" << msg;" << endl;
                output << "  " << function.second->getReturnType()->getName() << " retval;" << endl;
                output << "  " << getDecDecl(function.second->getReturnType()) << "(&retval, base64_decode(msg));" << endl;
                output << "  return retval;" << endl;
            } else {
                output << "  *GRADING << \"Void function " << function.first << " returned\";" << endl;
                output << "  return;" << endl;
            }
            output << "}" << endl << endl;
        }

        return 0;
    }

    int writeProxyReadFromStream(stringstream& output) {
        output << "string readFromStream() {\n";
        output << "  stringstream name;          // name to build\n";
        output << "  char bufc;                  // next char to read\n";
        output << "  ssize_t readlen;            // amount of data read from socket\n\n";

        output << "  while(1) {\n";
        output << "    readlen = RPCPROXYSOCKET-> read(&bufc, 1);  // read a byte\n";

        output << "    //\n";
        output << "    // With TCP streams, we should never get a 0 length read except with\n";
        output << "    // timeouts (which we're not setting in pingstreamserver) or EOF\n";
        output << "    //\n";
        output << "    if (readlen == 0) { \n";
        output << "      c150debug->printf(C150RPCDEBUG,\"stub: read zero length message, checking EOF\");\n";
        output << "      if (RPCPROXYSOCKET-> eof()) {\n";
        output << "        c150debug->printf(C150RPCDEBUG, \"stub: EOF signaled on input\");\n";
        output << "        return name.str();\n";
        output << "      } else { \n";
        output << "          throw C150Exception(\"stub: unexpected zero length read without eof\");\n";
        output << "      }\n";
        output << "    }\n\n";

        output << "    // check for null or space\n";
        output << "    if (bufc == '\\0') {\n";
        output << "      return name.str();\n";
        output << "    }\n";
        output << "    name << bufc;\n";
        output << "  }\n";
        output << "  throw C150Exception(\"readFromString: This should never be thrown.\");\n";
        output << "}\n";

        return 0;
    }
}