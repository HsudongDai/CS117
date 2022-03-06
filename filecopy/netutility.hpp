// Written by the author himself
// execute the basic functions for file-transmitting
// as a library that can be linked to other files
// Author: Xudong Dai
// Date: 2022-02-26
// declare the function of a file-receiving server

#ifndef __NETUTILITY_H_INCLUDED__
#define __NETUTILITY_H_INCLUDED__

#include <map>
#include <tuple>
#include <string>
#include <vector>
#include <fstream>
#include "c150dgmsocket.h"
#include "c150nastydgmsocket.h"
#include "c150debug.h"
#include "c150grading.h"


namespace C150NETWORK {

    // predefine the struct of a complete data packet
    typedef std::tuple<int, int, std::string, int, int, std::vector<char>> Packet;

    // write/read each data packet from the socket
    // low-level interfaces
    int sendMessage(C150DgmSocket* sock, int messageType, const string fileName, int packetID, int carryloadLen, const char* fileBuffer, int isClient);
    Packet receiveMessage(C150DgmSocket* sock);

    // high-level interfaces
    // uses previous 2 functions to build up the full function
    Packet receiveFileBySock(C150DgmSocket* sock, map<string, vector<char>>& fileQueue, Packet& prevPack);
    int sendFileBySock(C150DgmSocket* sock, string filename, vector<char>& fileBuffer);

    // tool function
    // extract a binary packet into an organized Packet
    Packet arrayToPacket(vector<char>& array);
}

#endif
