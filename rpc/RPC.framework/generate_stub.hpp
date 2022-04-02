#ifndef __GENERATE_STUB_HPP__
#define __GENERATE_STUB_HPP__

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

#include "idl_to_json.hpp"

using namespace std;
using namespace C150NETWORK;

namespace C150NETWORK{
    int writeStubHeader(stringstream& output, const char idl_filename[]);
    int writeStructDefinitions(stringstream& output, const Declarations& parseTree);
    int writeStubFunctions(stringstream& output, const Declarations& parseTree);
    int writeStubBadFunction(stringstream& output, const char idl_filename[]);
    int writeStubDispatcher(stringstream& output, const Declarations& parseTree);
    int writeStubGetFunctionNameFromStream(stringstream& output, const char idl_filename[]);
    int generateStub(const char idl_filename[], const char outputFilepath[]);
}

#endif