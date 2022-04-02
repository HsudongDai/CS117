#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <map>
#include "c150debug.h"
#include "c150exceptions.h"
#include "declarations.h"
#include "functiondeclaration.h"
#include "typedeclaration.h"

#include "idl_to_json.hpp"

#include "generate_proxy.hpp"
#include "generate_stub.hpp"

using namespace std;
using namespace C150NETWORK;

int rpcGenerate(const char idl_filename[], const char filepath[]) {
    try {
        if (idl_filename == nullptr || strlen(idl_filename) == 0) {
            throw C150Exception("rpcGenerate: idl_filename or is NULL or empty");
        }
        if (filepath == nullptr || strlen(filepath) == 0) {
            throw C150Exception("rpcGenerate: filepath or is NULL or empty");
        }

        if (generateStub(idl_filename, filepath) != 0) {
            throw C150Exception("rpcGenerate: generateStub failed");
        }
        if (generateProxy(idl_filename, filepath) != 0) {
            throw C150Exception("rpcGenerate: generateProxy failed");
        }
    } catch (C150Exception& e) {
        cerr << "rpcGenerate: " << e.formattedExplanation() << endl;
        return -1;
    }

    return 0;
}

void setUpDebugLogging(const char *logname, int argc, char *argv[]) {

    //   
    //           Choose where debug output should go
    //
    // The default is that debug output goes to cerr.
    //
    // Uncomment the following three lines to direct
    // debug output to a file. Comment them
    // to default to the console.
    //
    ofstream *outstreamp = new ofstream(logname);
    DebugStream *filestreamp = new DebugStream(outstreamp);
    DebugStream::setDefaultLogger(filestreamp);

    //
    //  Put the program name and a timestamp on each line of the debug log.
    //
    c150debug->setPrefix(argv[0]);
    c150debug->enableTimestamp(); 

    //
    // Ask to receive all classes of debug message
    //
    // See c150debug.h for other classes you can enable. To get more than
    // one class, you can or (|) the flags together and pass the combined
    // mask to c150debug -> enableLogging 
    //
    // By the way, the default is to disable all output except for
    // messages written with the C150ALWAYSLOG flag. Those are typically
    // used only for things like fatal errors. So, the default is
    // for the system to run quietly without producing debug output.
    //
    c150debug->enableLogging(C150APPLICATION | C150NETWORKTRAFFIC | 
                             C150NETWORKDELIVERY | C150RPCDEBUG); 
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <idl_filename> <output_filepath>" << endl;
        return -1;
    }

    setUpDebugLogging("rpcgenerate.log", argc, argv);

    return rpcGenerate(argv[1], argv[2]);
}