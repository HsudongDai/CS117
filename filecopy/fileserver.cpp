// modified from pingserver
// Author: Xudong Dai
// Date: 2022-02-26
// execute the function of a file-receiving server

#include <map>
#include "c150nastydgmsocket.h"
#include "c150debug.h"
#include "sha1.hpp"
#include "copyfile.hpp"
#include "netutility.hpp"
#include <fstream>
#include <cstdlib> 


using namespace C150NETWORK;  // for all the comp150 utilities 

// 

const int networkNastinessArg = 1;
const int fileNastinessArg = 2;
const int targetDirArg = 3;

void setUpDebugLogging(const char *logname, int argc, char *argv[]);


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//
//                           main program
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
 
int
main(int argc, char *argv[])
{
    GRADEME(argc, argv);
    //
    // Variable declarations
    //
    ssize_t readlen;             // amount of data read from socket
    char incomingMessage[512];   // received message data

    //
    // Check command line and parse arguments
    //
    if (argc != 4)  {
        fprintf(stderr,"Correct syntxt is: %s <network_nastiness> <file_nastiness> <target_dir>\n", argv[0]);
        exit(1);
    }
    // strspn used to check whether argc is a number
    if (strspn(argv[networkNastinessArg], "0123456789") != strlen(argv[networkNastinessArg])) {
        fprintf(stderr,"Network Nastiness %s is not numeric\n", argv[networkNastinessArg]);     
        fprintf(stderr,"Correct syntxt is: %s <network_nastiness> <file_nastiness> <target_dir>\n", argv[0]);     
        exit(4);
    }

     if (strspn(argv[fileNastinessArg], "0123456789") != strlen(argv[fileNastinessArg])) {
        fprintf(stderr,"File Nastiness %s is not numeric\n", argv[fileNastinessArg]);     
        fprintf(stderr,"Correct syntxt is: %s <network_nastiness> <file_nastiness> <target_dir>\n", argv[0]);     
        exit(4);
    }

    int32_t networkNastiness = atoi(argv[networkNastinessArg]);
    int32_t fileNastiness = atoi(argv[fileNastinessArg]);

    if (networkNastiness < 0 || networkNastiness > 4) {
        fprintf(stderr,"Network Nastiness %s must be between 0 and 4, while given %d\n", networkNastiness);
    }

    if (fileNastiness < 0 || fileNastiness > 5) {
        fprintf(stderr,"File Nastiness %s must be between 0 and 5, while given %d\n", fileNastiness);
    }
    
    // convert the target directory into a string object 
    const string target(argv[targetDirArg], argv[targetDirArg] + strlen(argv[targetDirArg]));
    //
    //  Set up debug message logging
    //
    setUpDebugLogging("fileserver_debug.txt", argc, argv);

    //
    // We set a debug output indent in the server only, not the client.
    // That way, if we run both programs and merge the logs this way:
    //
    //    cat pingserverdebug.txt pingserverclient.txt | sort
    //
    // it will be easy to tell the server and client entries apart.
    //
    // Note that the above trick works because at the start of each
    // log entry is a timestamp that sort will indeed arrange in 
    // timestamp order, thus merging the logs by time across 
    // server and client.
    //
    c150debug->setIndent("    ");              // if we merge client and server
    // logs, server stuff will be indented
    map<string, vector<char>> fileQueue;
    //
    // Create socket, loop receiving and responding
    //
    try {
        //
        // Create the socket
	    //
        c150debug->printf(C150APPLICATION,"Creating C150NastyDgmSocket(nastiness=%d)",
                          networkNastiness);
        C150DgmSocket *sock = new C150NastyDgmSocket(networkNastiness);
        c150debug->printf(C150APPLICATION,"Ready to accept messages");

        // create an empty packet as the initializer
        // hence the following packet can compare to it
        string emptyFilename;
        vector<char> emptyData;
        Packet prevPack = make_tuple(0, 0, emptyFilename, 0, 0, emptyData);
        //
        // infinite loop processing messages
        //
        while (true) {
            string statusCode;

            // when server send message to 
            prevPack = receiveFileBySock(sock, fileQueue, prevPack);

            int messageType = get<0>(prevPack);
            string filename = get<2>(prevPack);

            if (messageType == 1) {
                *GRADING << filename << " starting to receive file." << endl;
            }
            // when message type is 16, it means the file-sending process is finished
            // the server can write back the file now
            if (messageType == 16) {
                cout << "Got buffer size: " << fileQueue[filename].size()<< endl;
                *GRADING << filename << " received, beginning end-to-end check." << endl;
                safeWriteFile(target, filename, fileQueue[filename], fileNastiness);
                c150debug->printf(C150APPLICATION,"Successfully write file %s", filename);
                cout << "receive: " << filename << endl;
            }
        } 
    }

    catch (C150NetworkException& e) {
        // Write to debug log
        c150debug->printf(C150ALWAYSLOG,"Caught C150NetworkException: %s\n",
                          e.formattedExplanation().c_str());
        // In case we're logging to a file, write to the console too
        cerr << argv[0] << ": caught C150NetworkException: " << e.formattedExplanation() << endl;
    }

    // This only executes if there was an error caught above
    return 4;
}



// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//
//                     setUpDebugLogging
//
//        For COMP 150-IDS, a set of standards utilities
//        are provided for logging timestamped debug messages.
//        You can use them to write your own messages, but 
//        more importantly, the communication libraries provided
//        to you will write into the same logs.
//
//        As shown below, you can use the enableLogging
//        method to choose which classes of messages will show up:
//        You may want to turn on a lot for some debugging, then
//        turn off some when it gets too noisy and your core code is
//        working. You can also make up and use your own flags
//        to create different classes of debug output within your
//        application code
//
//        NEEDSWORK: should be factored and shared w/pingclient
//        NEEDSWORK: document arguments
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
 
void setUpDebugLogging(const char *logname, int argc, char *argv[]) {

    //   
    //           Choose where debug output should go
    //
    // The default is that debug output goes to cerr.
    //
    // Uncomment the following three lines to direct
    // debug output to a file. Comment them to 
    // default to the console
    //  
    // Note: the new DebugStream and ofstream MUST live after we return
    // from setUpDebugLogging, so we have to allocate
    // them dynamically.
    //
    //
    // Explanation: 
    // 
    //     The first line is ordinary C++ to open a file
    //     as an output stream.
    //
    //     The second line wraps that will all the services
    //     of a comp 150-IDS debug stream, and names that filestreamp.
    //
    //     The third line replaces the global variable c150debug
    //     and sets it to point to the new debugstream. Since c150debug
    //     is what all the c150 debug routines use to find the debug stream,
    //     you've now effectively overridden the default.
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
                             C150NETWORKDELIVERY); 
}
