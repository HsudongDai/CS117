#ifndef __GENERATE_PROXY_HPP__
#define __GENERATE_PROXY_HPP__

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

namespace C150NETWORK {
    int writeProxyHeader(stringstream& output, const char idl_filename[]);
    int writeProxyStructDefinitions(stringstream& output, const Declarations& parseTree);
    int writeProxyTypeParsers(stringstream& output, const Declarations& parseTree);
    int writeProxyFunctions(stringstream& output, const Declarations& parseTree, const char idl_filename[]);
    int generateProxy(const char idl_filename[], const char outputFilepath[]); 
}

#endif