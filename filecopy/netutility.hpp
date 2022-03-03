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
    typedef std::tuple<int, int, std::string, int, int, std::vector<char>> Packet;

    int sendMessage(C150DgmSocket* sock, int messageType, const string fileName, int packetID, int carryloadLen, const char* fileBuffer, int isClient);

    Packet receiveMessage(C150DgmSocket* sock);

    Packet receiveFileBySock(C150DgmSocket* sock, map<string, string>& fileQueue, Packet& prevPack);

    int sendFileBySock(C150DgmSocket* sock, string filename, vector<char>& fileBuffer);

    Packet arrayToPacket(vector<char>& array);
}

#endif
